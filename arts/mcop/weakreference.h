#ifndef MCOP_WEAKREFERENCE_H
#define MCOP_WEAKREFERENCE_H

namespace Arts {

class WeakReferenceBase {
public:
	virtual void release() = 0;
};

/**
 * The WeakReference class can be used to savely store an object, without
 * disallowing that it gets destroyed. A typical situation where you may want
 * to use this is when you implement a datastructure, where A refers B, and
 * B refers A. Using "normal" references, this structure would never get
 * freed.
 *
 * Usage example:
 *
 *     Synth_WAVE_SIN sin;
 *     WeakReference<Synth_PLAY_WAV> weak_sin;
 *
 *     {
 *       Synth_WAVE_SIN test = weak_sin;
 *         if(test.isNull())
 *           printf("#1 missing object\n");
 *         else
 *           test.start();
 *     }
 *
 *     // now, the strong reference leaves
 *     sin = Synth_WAVE_SIN::null();
 *
 *     {
 *       Synth_WAVE_SIN test = weak_sin;
 *	     if(test.isNull())
 *         printf("#2 missing object\n");
 *       else
 *         test.stop();
 *
 * This would output "#2 missing object".
 *
 */

template<class SmartWrapper>
class WeakReference : public WeakReferenceBase
{
private:
	typename SmartWrapper::_base_class *content;

public:
	WeakReference()
	{
		content = 0;
	}
	WeakReference(const WeakReference<SmartWrapper>& source)
		:WeakReferenceBase(source), content(source.content)
	{
		if(content)
			content->_addWeakReference(this);
	}
	WeakReference(SmartWrapper& source)
	{
		content = source._base();
		if(content)
			content->_addWeakReference(this);
	}
	inline WeakReference<SmartWrapper>& operator=(const
						WeakReference<SmartWrapper>& source)
	{
		release();
		content = source.content;
		if(content)
			content->_addWeakReference(this);
		return *this;
	}
	virtual ~WeakReference()
	{
		release();
	}
	void release()
	{
		if(content)
			content->_removeWeakReference(this);
	}
	inline operator SmartWrapper() const
	{
		if(content)
			return SmartWrapper::_from_base(content->_copy());
		else
			return SmartWrapper::null();
	}
};

};

#endif /* MCOP_WEAKREFERENCE_H */
