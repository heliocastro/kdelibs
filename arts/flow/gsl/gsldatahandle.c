/* GSL - Generic Sound Layer
 * Copyright (C) 2001 Tim Janik
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#include "gsldatahandle.h"

#include "gslcommon.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>


/* --- typedefs --- */
typedef struct {
  GslDataHandle     dhandle;
  GslDataHandle	   *src_handle;
  GslLong	    cut_offset;
  GslLong	    n_cut_values;
} CutHandle;
typedef struct {
  GslDataHandle     dhandle;
  GslWaveFormatType format;
  guint             byte_order;
  guint             boffset_reminder;
  gint              fd;
} WaveHandle;


/* --- variables --- */
static GslRing *wave_handles = NULL;
static GslMutex dhandle_global = { 0, };


/* --- standard functions --- */
void
_gsl_init_data_handles (void)
{
  static gboolean initialized = FALSE;

  g_assert (initialized == FALSE);
  initialized = TRUE;

  gsl_mutex_init (&dhandle_global);
}

GslDataHandle*
gsl_data_handle_ref (GslDataHandle *dhandle)
{
  g_return_val_if_fail (dhandle != NULL, NULL);
  g_return_val_if_fail (dhandle->ref_count > 0, NULL);

  GSL_SPIN_LOCK (&dhandle->mutex);
  dhandle->ref_count++;
  GSL_SPIN_UNLOCK (&dhandle->mutex);

  return dhandle;
}

void
gsl_data_handle_common_free (GslDataHandle *dhandle)
{
  g_return_if_fail (dhandle != NULL);
  g_return_if_fail (dhandle->vtable != NULL);
  g_return_if_fail (dhandle->ref_count == 0);

  g_free (dhandle->name);
  dhandle->name = NULL;
  gsl_mutex_destroy (&dhandle->mutex);
}

void
gsl_data_handle_unref (GslDataHandle *dhandle)
{
  gboolean destroy;

  g_return_if_fail (dhandle != NULL);
  g_return_if_fail (dhandle->ref_count > 0);

  GSL_SPIN_LOCK (&dhandle->mutex);
  dhandle->ref_count--;
  destroy = dhandle->ref_count == 0;
  GSL_SPIN_UNLOCK (&dhandle->mutex);
  if (destroy)
    dhandle->vtable->destroy (dhandle);
}

GslErrorType
gsl_data_handle_open (GslDataHandle *dhandle)
{
  g_return_val_if_fail (dhandle != NULL, GSL_ERROR_INTERNAL);
  g_return_val_if_fail (dhandle->ref_count > 0, GSL_ERROR_INTERNAL);

  GSL_SPIN_LOCK (&dhandle->mutex);
  if (dhandle->open_count == 0)
    {
      GslErrorType error = dhandle->vtable->open (dhandle);

      if (error)
	{
	  GSL_SPIN_UNLOCK (&dhandle->mutex);
	  return error;
	}
      dhandle->ref_count++;
      dhandle->open_count++;
    }
  else
    dhandle->open_count++;
  GSL_SPIN_UNLOCK (&dhandle->mutex);

  return GSL_ERROR_NONE;
}

void
gsl_data_handle_close (GslDataHandle *dhandle)
{
  gboolean need_unref;

  g_return_if_fail (dhandle != NULL);
  g_return_if_fail (dhandle->ref_count > 0);
  g_return_if_fail (dhandle->open_count > 0);

  GSL_SPIN_LOCK (&dhandle->mutex);
  dhandle->open_count--;
  need_unref = !dhandle->open_count;
  if (!dhandle->open_count)
    dhandle->vtable->close (dhandle);
  GSL_SPIN_UNLOCK (&dhandle->mutex);
  if (need_unref)
    gsl_data_handle_unref (dhandle);
}

GslLong
gsl_data_handle_read (GslDataHandle *dhandle,
		      GslLong        value_offset,
		      GslLong        n_values,
		      gfloat        *values)
{
  GslLong l;

  g_return_val_if_fail (dhandle != NULL, -1);
  g_return_val_if_fail (dhandle->ref_count > 0, -1);
  g_return_val_if_fail (dhandle->open_count > 0, -1);
  g_return_val_if_fail (value_offset >= 0, -1);
  if (n_values < 1)
    return 0;
  g_return_val_if_fail (values != NULL, -1);
  g_return_val_if_fail (value_offset + n_values <= dhandle->n_values, -1);
  
  GSL_SPIN_LOCK (&dhandle->mutex);
  l = dhandle->vtable->read (dhandle, value_offset, n_values, values);
  GSL_SPIN_UNLOCK (&dhandle->mutex);

  return l;
}

gboolean
gsl_data_handle_common_init (GslDataHandle *dhandle,
			     const gchar   *file_name)
{
  g_return_val_if_fail (dhandle != NULL, FALSE);
  g_return_val_if_fail (dhandle->vtable == NULL, FALSE);
  g_return_val_if_fail (dhandle->name == NULL, FALSE);
  g_return_val_if_fail (dhandle->ref_count == 0, FALSE);

  if (file_name)
    {
      struct stat statbuf = { 0, };

      if (stat (file_name, &statbuf) < 0 || statbuf.st_size < 1)
	return FALSE;

      dhandle->name = g_strdup (file_name);
      dhandle->mtime = statbuf.st_mtime;
      dhandle->n_values = statbuf.st_size;
    }
  else
    {
      dhandle->name = NULL;
      dhandle->mtime = time (NULL);
      dhandle->n_values = 0;
    }
  gsl_mutex_init (&dhandle->mutex);
  dhandle->ref_count = 1;
  dhandle->open_count = 0;

  return TRUE;
}


/* --- cut handle --- */
static void
cut_handle_destroy (GslDataHandle *data_handle)
{
  CutHandle *chandle = (CutHandle*) data_handle;

  gsl_data_handle_unref (chandle->src_handle);

  gsl_data_handle_common_free (data_handle);
  gsl_delete_struct (CutHandle, 1, chandle);
}

static GslErrorType
cut_handle_open (GslDataHandle *data_handle)
{
  CutHandle *chandle = (CutHandle*) data_handle;

  return gsl_data_handle_open (chandle->src_handle);
}

static void
cut_handle_close (GslDataHandle *data_handle)
{
  CutHandle *chandle = (CutHandle*) data_handle;

  gsl_data_handle_close (chandle->src_handle);
}

static GslLong
cut_handle_read (GslDataHandle *data_handle,
		 GslLong        voffset,
		 GslLong        n_values,
		 gfloat        *values)
{
  CutHandle *chandle = (CutHandle*) data_handle;

  if (voffset < chandle->cut_offset)
    return gsl_data_handle_read (chandle->src_handle, voffset,
				 MIN (chandle->cut_offset - voffset, n_values),
				 values);
  else
    return gsl_data_handle_read (chandle->src_handle, voffset + chandle->n_cut_values,
				 n_values, values);
}

GslDataHandle*
gsl_data_handle_new_translate (GslDataHandle *src_handle,
			       GslLong        cut_offset,
			       GslLong        n_cut_values,
			       GslLong	      tail_cut)
{
  static GslDataHandleFuncs cut_handle_vtable = {
    cut_handle_open,
    cut_handle_read,
    cut_handle_close,
    cut_handle_destroy,
  };
  CutHandle *chandle;
  gboolean success;

  g_return_val_if_fail (src_handle != NULL, NULL);
  g_return_val_if_fail (cut_offset >= 0 && n_cut_values >= 0 && tail_cut >= 0, NULL);
  g_return_val_if_fail (cut_offset < src_handle->n_values, NULL);
  g_return_val_if_fail (cut_offset + n_cut_values + tail_cut < src_handle->n_values, NULL);

  chandle = gsl_new_struct0 (CutHandle, 1);
  success = gsl_data_handle_common_init (&chandle->dhandle, NULL);
  if (success)
    {
      chandle->dhandle.name = g_strconcat (src_handle->name, "/ #translate /", NULL);
      chandle->dhandle.vtable = &cut_handle_vtable;
      chandle->src_handle = gsl_data_handle_ref (src_handle);
      chandle->cut_offset = cut_offset;
      chandle->n_cut_values = n_cut_values;
      chandle->dhandle.n_values = src_handle->n_values - n_cut_values - tail_cut;
    }
  if (!success)
    {
      gsl_delete_struct (CutHandle, 1, chandle);
      return NULL;
    }
  return &chandle->dhandle;
}


/* --- wave handle --- */
static guint
wave_format_width (GslWaveFormatType format)
{
  switch (format)
    {
    case GSL_WAVE_FORMAT_UNSIGNED_8:
    case GSL_WAVE_FORMAT_SIGNED_8:
      return 1;
    case GSL_WAVE_FORMAT_UNSIGNED_12:
    case GSL_WAVE_FORMAT_SIGNED_12:
    case GSL_WAVE_FORMAT_UNSIGNED_16:
    case GSL_WAVE_FORMAT_SIGNED_16:
      return 2;
    case GSL_WAVE_FORMAT_FLOAT:
      return 4;
    default:
      return 0;
    }
}

static void
wave_handle_destroy (GslDataHandle *data_handle)
{
  WaveHandle *whandle = (WaveHandle*) data_handle;

  GSL_SPIN_LOCK (&dhandle_global);
  wave_handles = gsl_ring_remove (wave_handles, whandle);
  GSL_SPIN_UNLOCK (&dhandle_global);

  gsl_data_handle_common_free (data_handle);
  gsl_delete_struct (WaveHandle, 1, whandle);
}

static GslErrorType
wave_handle_open (GslDataHandle *data_handle)
{
  WaveHandle *whandle = (WaveHandle*) data_handle;

  whandle->fd = open (whandle->dhandle.name, O_RDONLY);

  if (whandle->fd < 0)
    return GSL_ERROR_IO;

  return GSL_ERROR_NONE;
}

static void
wave_handle_close (GslDataHandle *data_handle)
{
  WaveHandle *whandle = (WaveHandle*) data_handle;

  close (whandle->fd);
  whandle->fd = -1;
}

static GslLong
wave_handle_read (GslDataHandle *data_handle,
		  GslLong        voffset,
		  GslLong        n_values,
		  gfloat        *values)
{
  WaveHandle *whandle = (WaveHandle*) data_handle;
  gpointer buffer = values;
  GslLong l, i = 0;

  i = voffset * wave_format_width (whandle->format);	/* float offset into bytes */
  i += whandle->boffset_reminder;			/* remaining byte offset */
  l = lseek (whandle->fd, i, SEEK_SET);
  if (l < 0 && errno != EINVAL)
    return -1;
  if (l != i)
    return 0;

  switch (whandle->format)
    {
      guint8 *u8; gint8 *s8; guint16 *u16; guint32 *u32;
    case GSL_WAVE_FORMAT_UNSIGNED_8:
      u8 = buffer; u8 += n_values * 3;
      l = read (whandle->fd, u8, n_values);
      if (l < 1)
	return l;
      for (i = 0; i < l; i++)
	{
	  int v = u8[i] - 128;
	  values[i] = v * (1. / 128.);
	}
      break;
    case GSL_WAVE_FORMAT_SIGNED_8:
      s8 = buffer; s8 += n_values * 3;
      l = read (whandle->fd, s8, n_values);
      if (l < 1)
	return l;
      for (i = 0; i < l; i++)
	values[i] = s8[i] * (1. / 128.);
      break;
    case GSL_WAVE_FORMAT_SIGNED_12:
    case GSL_WAVE_FORMAT_UNSIGNED_12:
    case GSL_WAVE_FORMAT_SIGNED_16:
    case GSL_WAVE_FORMAT_UNSIGNED_16:
      u16 = buffer; u16 += n_values;
      l = read (whandle->fd, u16, n_values << 1);
      if (l < 2)
	return l < 0 ? l : 0;
      l >>= 1;
      switch (whandle->format)
	{
	case GSL_WAVE_FORMAT_UNSIGNED_16:
	  if (whandle->byte_order != GSL_BYTE_ORDER)
	    for (i = 0; i < l; i++)
	      {
		int v = GUINT16_SWAP_LE_BE (u16[i]); v -= 32768;
		values[i] = v * (1. / 32768.);
	      }
	  else /* whandle->byte_order == GSL_BYTE_ORDER */
	    for (i = 0; i < l; i++)
	      {
		int v = u16[i]; v -= 32768;
		values[i] = v * (1. / 32768.);
	      }
	  break;
	case GSL_WAVE_FORMAT_UNSIGNED_12:
	  if (whandle->byte_order != GSL_BYTE_ORDER)
	    for (i = 0; i < l; i++)
	      {
		int v = GUINT16_SWAP_LE_BE (u16[i]); v &= 0x0fff; v -= 4096;
		values[i] = v * (1. / 4096.);
	      }
	  else /* whandle->byte_order == GSL_BYTE_ORDER */
	    for (i = 0; i < l; i++)
	      {
		int v = u16[i]; v &= 0x0fff; v -= 4096;
		values[i] = v * (1. / 4096.);
	      }
	  break;
	case GSL_WAVE_FORMAT_SIGNED_16:
	  if (whandle->byte_order != GSL_BYTE_ORDER)
	    for (i = 0; i < l; i++)
	      {
		gint16 v = GUINT16_SWAP_LE_BE (u16[i]);
		values[i] = v * (1. / 32768.);
	      }
	  else /* whandle->byte_order == GSL_BYTE_ORDER */
	    for (i = 0; i < l; i++)
	      {
		gint16 v = u16[i];
		values[i] = v * (1. / 32768.);
	      }
	  break;
	case GSL_WAVE_FORMAT_SIGNED_12:
	  if (whandle->byte_order != GSL_BYTE_ORDER)
	    for (i = 0; i < l; i++)
	      {
		gint16 v = GUINT16_SWAP_LE_BE (u16[i]);
		values[i] = CLAMP (v, -4096, 4096) * (1. / 4096.);
	      }
	  else /* whandle->byte_order == GSL_BYTE_ORDER */
	    for (i = 0; i < l; i++)
	      {
		gint16 v = u16[i];
		values[i] = CLAMP (v, -4096, 4096) * (1. / 4096.);
	      }
	  break;
	default:
	  g_assert_not_reached ();
	}
      break;
    case GSL_WAVE_FORMAT_FLOAT:
      u32 = buffer;
      l = read (whandle->fd, u32, n_values << 2);
      if (l < 4)
	return l < 0 ? l : 0;
      l >>= 2;
      if (whandle->byte_order != GSL_BYTE_ORDER)
	for (i = 0; i < l; i++)
	  u32[i] = GUINT32_SWAP_LE_BE (u32[i]);
      break;
    default:
      g_assert_not_reached ();
    }

  return l;
}

GslDataHandle*
gsl_wave_handle_new_cached (const gchar      *file_name,
			    GTime             mtime,
			    GslWaveFormatType format,
			    guint	      byte_order,
			    guint	      boffset_reminder)
{
  static GslDataHandleFuncs wave_handle_vtable = {
    wave_handle_open,
    wave_handle_read,
    wave_handle_close,
    wave_handle_destroy,
  };
  WaveHandle *whandle;
  GslRing *ring;
  gboolean success;
  
  g_return_val_if_fail (file_name != NULL, NULL);
  g_return_val_if_fail (format > GSL_WAVE_FORMAT_NONE && format < GSL_WAVE_FORMAT_LAST, NULL);
  g_return_val_if_fail (byte_order == G_LITTLE_ENDIAN || byte_order == G_BIG_ENDIAN, NULL);
  g_return_val_if_fail (boffset_reminder < wave_format_width (format), NULL);

  if (mtime == 0)	/* don't need mtime match */
    {
      struct stat statbuf = { 0, };
      
      if (stat (file_name, &statbuf) < 0 || statbuf.st_size < 1)
	return NULL;
      mtime = statbuf.st_mtime;
    }

  GSL_SPIN_LOCK (&dhandle_global);
  for (ring = wave_handles; ring; ring = gsl_ring_walk (wave_handles, ring))
    {
      whandle = ring->data;

      if (whandle->dhandle.mtime == mtime &&
	  strcmp (whandle->dhandle.name, file_name) == 0 &&
	  whandle->format == format && whandle->byte_order == byte_order &&
	  whandle->boffset_reminder == boffset_reminder)
	{
	  gsl_data_handle_ref (&whandle->dhandle);
	  GSL_SPIN_UNLOCK (&dhandle_global);
	  return &whandle->dhandle;
	}
    }
  GSL_SPIN_UNLOCK (&dhandle_global);

  whandle = gsl_new_struct0 (WaveHandle, 1);
  success = gsl_data_handle_common_init (&whandle->dhandle, file_name);
  if (success)
    {
      GslLong l;

      whandle->dhandle.vtable = &wave_handle_vtable;
      whandle->format = format;
      whandle->byte_order = byte_order;
      whandle->boffset_reminder = boffset_reminder;
      whandle->fd = -1;

      /* convert size into n_values, i.e. float length */
      l = whandle->dhandle.n_values;
      if (l > whandle->boffset_reminder)
	{
	  l -= whandle->boffset_reminder;
	  l /= wave_format_width (format);
	  if (l > 0)
	    whandle->dhandle.n_values = l;
	  else
	    success = FALSE;
	}
      else
	success = FALSE;
    }
  if (!success)
    {
      gsl_delete_struct (WaveHandle, 1, whandle);
      return NULL;
    }
  GSL_SPIN_LOCK (&dhandle_global);
  wave_handles = gsl_ring_append (wave_handles, whandle);
  GSL_SPIN_UNLOCK (&dhandle_global);

  return &whandle->dhandle;
}

GslDataHandle*
gsl_wave_handle_new (const gchar      *file_name,
		     GTime             mtime, /* may be 0 */
		     GslWaveFormatType format,
		     guint             byte_order,
		     GslLong           byte_offset,
		     GslLong           n_values)
{
  GslDataHandle *whandle, *thandle;
  guint fwidth, boffset_reminder;
  GslLong offset;

  g_return_val_if_fail (file_name != NULL, NULL);
  g_return_val_if_fail (format > GSL_WAVE_FORMAT_NONE && format < GSL_WAVE_FORMAT_LAST, NULL);
  g_return_val_if_fail (byte_order == G_LITTLE_ENDIAN || byte_order == G_BIG_ENDIAN, NULL);
  g_return_val_if_fail (byte_offset >= 0, NULL);
  g_return_val_if_fail (n_values >= 1 || n_values == -1, NULL);

  fwidth = wave_format_width (format);
  boffset_reminder = byte_offset % fwidth;
  whandle = gsl_wave_handle_new_cached (file_name, mtime, format, byte_order, boffset_reminder);
  if (!whandle)
    return NULL;
  offset = (byte_offset - boffset_reminder) / fwidth;
  if (n_values == -1)
    n_values = whandle->n_values - offset;
  if (offset + n_values > whandle->n_values)
    {
      gsl_data_handle_unref (whandle);
      return NULL;
    }
  thandle = gsl_data_handle_new_translate (whandle,
					   0, offset,
					   whandle->n_values - offset - n_values);
  gsl_data_handle_unref (whandle);
  g_assert (thandle->n_values == n_values);	/* paranoid */

  return thandle;
}
