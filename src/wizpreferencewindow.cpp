#include "wizpreferencewindow.h"

#include "wiznotesettings.h"
#include "share/wizmisc.h"

CWizPreferenceWindow::CWizPreferenceWindow(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::CWizPreferenceWindow)
    , m_bRestart(false)
{
    ui->setupUi(this);

    setWindowTitle(tr("Preference"));

    connect(ui->buttonOK, SIGNAL(clicked()), this, SLOT(accept()));

    // general tab
    ui->comboLang->addItem(tr("Chinese"));
    ui->comboLang->addItem(tr("English"));


    // just hide skin setup on mac for convience.
#ifdef Q_WS_MAC
    ui->groupBoxSkin->hide();
#endif // Q_WS_MAC

    CString strCurrSkinName = ::WizGetSkinName();
    std::map<CString, CString> skins;
    typedef std::map<CString, CString>::value_type SKIN;
    ::WizGetSkins(skins);
    foreach (const SKIN& skin, skins)
    {
        ui->comboSkin->addItem(skin.second);
        if (strCurrSkinName == skin.first)
        {
            ui->comboSkin->setCurrentIndex(ui->comboSkin->count() - 1);
        }
    }

    connect(ui->comboSkin, SIGNAL(currentIndexChanged(QString)), SLOT(on_comboSkin_currentIndexChanged(QString)));
    //connect(m_labelRestartForSkin, SIGNAL(linkActivated(QString)), SLOT(on_labelRestartForSkin_linkActivated(QString)));

    // reading tab
    switch (::WizGetDefaultNoteView())
    {
    case viewmodeAlwaysEditing:
        ui->radioAlwaysEditing->setChecked(true);
        break;
    case viewmodeAlwaysReading:
        ui->radioAlwaysReading->setChecked(true);
        break;
    default:
        ui->radioAuto->setChecked(true);
        break;
    }

    connect(ui->radioAuto, SIGNAL(clicked(bool)), SLOT(on_radioAuto_clicked(bool)));
    connect(ui->radioAlwaysReading, SIGNAL(clicked(bool)), SLOT(on_radioAlwaysReading_clicked(bool)));
    connect(ui->radioAlwaysEditing, SIGNAL(clicked(bool)), SLOT(on_radioAlwaysEditing_clicked(bool)));

    // syncing tab
    ui->checkAutoSync->setChecked(::WizIsAutoSync());
    ui->checkDownloadAllNotesData->setChecked(::WizIsDownloadAllNotesData());

    QString proxySettings = WizFormatString1("<a href=\"proxy_settings\">%1</a>", tr("Proxy settings"));
    ui->labelProxySettings->setText(proxySettings);

    connect(ui->checkAutoSync, SIGNAL(clicked(bool)), SLOT(on_checkAutoSync_clicked(bool)));
    connect(ui->checkDownloadAllNotesData, SIGNAL(clicked(bool)), SLOT(on_checkDownloadAllNotesData_clicked(bool)));
    connect(ui->labelProxySettings, SIGNAL(linkActivated(QString)), SLOT(on_labelProxy_linkActivated(QString)));
}

void CWizPreferenceWindow::on_comboSkin_currentIndexChanged(const QString& text)
{
    ::WizSetSkinDisplayName(text);
    //m_labelRestartForSkin->setVisible(true);
    m_bRestart = true;
    emit settingsChanged(wizoptionsSkin);
}

void CWizPreferenceWindow::on_radioAuto_clicked(bool chcked)
{
    if (chcked)
    {
        ::WizSetDefaultNoteView(viewmodeKeep);
        emit settingsChanged(wizoptionsNoteView);
    }
}

void CWizPreferenceWindow::on_radioAlwaysReading_clicked(bool chcked)
{
    if (chcked)
    {
        ::WizSetDefaultNoteView(viewmodeAlwaysReading);
        emit settingsChanged(wizoptionsNoteView);
    }
}

void CWizPreferenceWindow::on_radioAlwaysEditing_clicked(bool chcked)
{
    if (chcked)
    {
        ::WizSetDefaultNoteView(viewmodeAlwaysEditing);
        emit settingsChanged(wizoptionsNoteView);
    }
}

void CWizPreferenceWindow::on_checkAutoSync_clicked(bool checked)
{
    ::WizSetAutoSync(checked);
    emit settingsChanged(wizoptionsSync);
}

void CWizPreferenceWindow::on_checkDownloadAllNotesData_clicked(bool checked)
{
    ::WizSetDownloadAllNotesData(checked);
    emit settingsChanged(wizoptionsSync);
}

void CWizPreferenceWindow::on_labelProxy_linkActivated(const QString & link)
{
    Q_UNUSED(link);

    ProxyDialog dlg(parentWidget());
    if (QDialog::Accepted != dlg.exec())
        return;

    emit settingsChanged(wizoptionsSync);
}

void CWizPreferenceWindow::accept()
{
    if (m_bRestart) {
        emit restartForSettings();
    }

    QDialog::accept();
}