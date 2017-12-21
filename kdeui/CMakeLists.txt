add_subdirectory(tests)

set(libkdeui_SRCS
    qxembed.cpp ksharedpixmap.cpp kthemestyle.cpp kurllabel.cpp knuminput.cpp
    ktoolbar.cpp klineedit.cpp klistview.cpp kprogress.cpp kimageeffect.cpp
    kcolordlg.cpp kselect.cpp kdatepik.cpp kdatetbl.cpp kfontdialog.cpp
    kpopupmenu.cpp ktabctl.cpp kstatusbar.cpp kmainwindow.cpp ktmainwindow.cpp
    kmenubar.cpp kpixmapio.cpp keditlistbox.cpp kseparator.cpp krestrictedline.cpp
    kcolorbtn.cpp ksystemtray.cpp kbuttonbox.cpp keditcl1.cpp keditcl2.cpp
    kled.cpp kdbtn.cpp kwizard.cpp kcontainer.cpp kkeydialog.cpp
    kruler.cpp kcursor.cpp kaccelmenu.cpp klineeditdlg.cpp kthemebase.cpp
    kcharselect.cpp kcolordrag.cpp knumvalidator.cpp kdialog.cpp kdialogbase.cpp
    kjanuswidget.cpp kaboutdialog.cpp kpixmapeffect.cpp kauthicon.cpp kmessagebox.cpp
    kdualcolorbtn.cpp ktoolbarradiogroup.cpp ktoolbarbutton.cpp ktextbrowser.cpp kaction.cpp
    khelpmenu.cpp kcmenumngr.cpp kpanelmenu.cpp kstdaction.cpp kdrawutil.cpp
    kiconview.cpp klistbox.cpp kbugreport.cpp kcombobox.cpp kpassdlg.cpp
    kxmlguiclient.cpp kxmlgui.cpp kxmlguibuilder.cpp kedittoolbar.cpp kdocktabctl.cpp
    kdockwidget.cpp kdockwidget_private.cpp kanimwidget.cpp krootpixmap.cpp kaboutkde.cpp
    kaboutapplication.cpp kpanelapplet.cpp kdcopactionproxy.cpp kcolorcombo.cpp kpushbutton.cpp
    kpanelextension.cpp kcompletionbox.cpp kalphapainter.cpp ksqueezedtextlabel.cpp kcommand.cpp
    kwindowlistmenu.cpp kfontcombo.cpp ktip.cpp kdatewidget.cpp karrowbutton.cpp
    )

set(libkdeui_HEADERS
    kprogress.h kcolordlg.h kcolordialog.h kselect.h kdatepik.h
    kdatetbl.h kfontdialog.h kpopupmenu.h ktabctl.h kstatusbar.h
    kimageeffect.h kmainwindow.h ktmainwindow.h ktoolbar.h kmenubar.h
    knuminput.h kseparator.h klineedit.h krestrictedline.h kcolorbutton.h
    kcolorbtn.h ksystemtray.h kdockwindow.h kbuttonbox.h kcontainer.h
    keditcl.h kled.h keditlistbox.h kdbtn.h kwizard.h
    kkeydialog.h kurllabel.h kruler.h kcursor.h kaccelmenu.h
    klineeditdlg.h kwidgetid.h kthemebase.h kthemestyle.h kcharselect.h
    kcolordrag.h qxembed.h knumvalidator.h kdialog.h kdialogbase.h
    kjanuswidget.h kaboutdialog.h kpixmapeffect.h kauthicon.h kmessagebox.h
    ksharedpixmap.h kdualcolorbtn.h ktoolbarbutton.h ktoolbarradiogroup.h ktextbrowser.h
    kaction.h khelpmenu.h kcmodule.h kcmenumngr.h kpanelmenu.h
    kstdaction.h kdrawutil.h kcombobox.h kiconview.h klistview.h
    klistbox.h kbugreport.h kpassdlg.h kxmlguiclient.h kxmlgui.h
    kxmlguibuilder.h kpixmapio.h kedittoolbar.h kdocktabctl.h kdockwidget.h
    kanimwidget.h krootpixmap.h kaboutkde.h kaboutapplication.h kpanelapplet.h
    kpushbutton.h kdcopactionproxy.h kcolorcombo.h kpanelextension.h kcompletionbox.h
    kalphapainter.h ksqueezedtextlabel.h kcommand.h kwindowlistmenu.h kfontcombo.h
    kaccelgen.h ktip.h kdatewidget.h karrowbutton.h
    )

set(libkdeui_NOINST_HEADERS
    kdockwidget_private.h klistviewlineedit.h kkeybutton.h kdialogbase_priv.h kaboutdialog_private.h
    kcursor_private.h kdepackages.h kdeui-doc.h
    )

qt2_wrap_cpp(libkdeui_SRCS
    SOURCES
        ${libkdeui_HEADERS}
        ${libkdeui_NOINST_HEADERS}
    )

install(FILES ${libkdeui_HEADERS} DESTINATION ${KDE2_INCLUDEDIR})

kde2_library(libkdeui
    SOURCES
        ${libkdeui_SRCS}
    LIBS
        kde2::kdecore
    VERSION
        3.0.0
   )

install(FILES
    aboutkde.png keys.png ktip-bulb.png ktip-background.png
    DESTINATION ${KDE2_DATADIR}/kdeui/pics
    )

install(FILES ui_standards.rc DESTINATION ${KDE2_CONFDIR}/ui)
