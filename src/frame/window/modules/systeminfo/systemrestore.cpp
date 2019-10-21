#include "systemrestore.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSaveFile>
#include <QDebug>
#include <QProcess>
#include <udisks2-qt5/ddiskdevice.h>
#include <udisks2-qt5/ddiskmanager.h>
#include <udisks2-qt5/dblockpartition.h>
#include <QScopedPointer>
#include <DDialog>
#include <DDBusSender>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::systeminfo;

DWIDGET_USE_NAMESPACE

SystemRestore::SystemRestore(QWidget* parent)
    : QWidget(parent) {
    m_restoreAllSettingBtn = new QPushButton;
    m_restoreAllSettingAndDataBtn = new QPushButton;
    QLabel* restoreAllSettingTip = new QLabel;
    QLabel* restoreAllSettingAndDataTip = new QLabel;

    restoreAllSettingTip->setText(tr("Initialize all data on your system partition and reset the system configuration file, but keep the username, password, and the contents of other partitions."));
    restoreAllSettingAndDataTip->setText(tr("Reinstall the system, which will erase all data from the disk"));
    restoreAllSettingTip->setWordWrap(true);
    restoreAllSettingAndDataTip->setWordWrap(true);

    m_restoreAllSettingBtn->setText(tr("Reset All Settings"));
    m_restoreAllSettingAndDataBtn->setText(tr("Erase All Content and Settings"));

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(m_restoreAllSettingBtn);
    mainLayout->addWidget(restoreAllSettingTip);
    mainLayout->addWidget(m_restoreAllSettingAndDataBtn);
    mainLayout->addWidget(restoreAllSettingAndDataTip);
    mainLayout->addStretch();

    setLayout(mainLayout);
    writeRestoreConfig(true);
}

void SystemRestore::writeRestoreConfig(bool formatData) const
{
    QJsonObject object{
        { "name", "restore" },
        { "message", "restore boot and root partition" },
        { "total", 4 },
        { "ask", true },
        {
            "tasks",
            QJsonArray{
                QJsonObject{ { "name", "task-1" },
                             { "message", "starting restore boot partition" },
                             { "progress", true },
                             { "command", "/bin/deepin-clone" },
                             { "env", QJsonObject{ { "source", "boot.dim" },
                                                   { "dest", "flag=boot" } } },
                             { "args", QJsonArray{ "boot.dim", "boot.info" } },
                             { "workdir", "/" } },
                QJsonObject{ { "name", "task-2" },
                             { "message", "starting restore root partition" },
                             { "progress", true },
                             { "command", "/bin/deepin-clone.wrapper" },
                             { "env", QJsonObject{ { "source", "system.dim" },
                                                   { "dest", "flag=rootfs" } } },
                             { "workdir", "/" } },
                QJsonObject{ { "name", "task-4" },
                             { "message", "regenerate /etc/fstab" },
                             { "progress", false },
                             { "command", "genfstab" },
                             { "env", QJsonObject{ { "boot", "flag=boot" },
                                                   { "root", "label=rootfs" },
                                                   { "data", "label=_dde_data" } } },
                             { "workdir", "/" } },
                QJsonObject{
                    { "name", "task-5" },
                    { "message", "fix bootloader order" },
                    { "progress", false },
                    { "command", "fix-bootloader" },
                } },
        }
    };

    if (formatData) {
        object["total"] = 5;
        object["tasks"] =
            object["tasks"].toArray()
            << QJsonObject{ { "name", "task-3" },
                            { "message", "formating data partition" },
                            { "progress", false },
                            { "command", "refresh-data-partition" },
                            { "env", QJsonObject{ { "source", "label=_dde_data" },
                                                  { "fstype", "ext4" } } },
                            { "workdir", "/" } };
    }

    QJsonDocument doc;
    doc.setObject(object);

    DDialog dialog;
    QString message{ tr(
        "This will reset all system settings to their defaults. Your data, username and "
        "password will not be deleted, please confirm and continue") };

    if (formatData) {
        message =
            tr("This will reinstall the system and clear all user data. It is risky, "
               "please confirm and continue");
    }

    dialog.setMessage(message);
    dialog.addButton(tr("Cancel"));

    {
        int result = dialog.addButton(tr("Confirm"), true, DDialog::ButtonWarning);
        if (dialog.exec() != result) {
            return;
        }
    }

    DDiskManager       manager;
    const QStringList& devices = manager.blockDevices();
    for (const QString& path : devices) {
        QScopedPointer<DBlockPartition> partition(
            DDiskManager::createBlockPartition(path));
        QScopedPointer<DBlockDevice> device(DDiskManager::createBlockDevice(path));
        if (device->idLabel() == "Backup") {
            device->unmount({});
            const QString& mountpoint = device->mount({});
            QSaveFile file("/tmp/playbook.json");
            file.open(QIODevice::WriteOnly | QIODevice::Text);
            file.write(doc.toJson());
            file.commit();

            QProcess process;
            process.start("pkexec", QStringList() << "mv" << "/tmp/playbook.json" << mountpoint);
            process.waitForFinished();
            break;
        }
    }

    DDialog reboot;
    reboot.setMessage(tr("You should reboot the computer to reset all settings, reboot now?"));
    reboot.addButton("Cancel");
    {
        int result = reboot.addButton("Confirm", true, DDialog::ButtonWarning);
        if (reboot.exec() != result) {
            return;
        }
    }

    DDBusSender()
        .service("com.deepin.dde.shutdownFront")
        .path("/com/deepin/dde/shutdownFront")
        .interface("com.deepin.dde.shutdownFront")
        .method("Restart")
        .call();
}
