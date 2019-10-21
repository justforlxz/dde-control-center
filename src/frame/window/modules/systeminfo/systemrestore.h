#pragma once

#include "window/namespace.h"

#include <QWidget>
#include <QPushButton>

namespace DCC_NAMESPACE {
namespace systeminfo {
class SystemRestore : public QWidget {
    Q_OBJECT
public:
    SystemRestore(QWidget* parent = nullptr);

private:
    void writeRestoreConfig(bool formatData) const;

private:
    QPushButton* m_restoreAllSettingBtn;
    QPushButton* m_restoreAllSettingAndDataBtn;
};
}  // namespace systeminfo
}  // namespace DCC_NAMESPACE
