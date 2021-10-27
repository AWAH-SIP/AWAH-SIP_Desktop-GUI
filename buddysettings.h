#ifndef BUDDYSETTINGS_H
#define BUDDYSETTINGS_H

#include <QDialog>
#include "cmdfacade.h"
#include "buddydialog.h"

class BuddyModel : public QAbstractTableModel
{
    Q_OBJECT;
public:
    BuddyModel(QObject *parent = nullptr, QList<s_buddy> *buddies = nullptr ,CmdFacade *lib = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void refresh();

private:
    QList<s_buddy> *m_BuddyList;
    CmdFacade *m_cmdFacade;

};



namespace Ui {
class BuddySettings;
}

class BuddySettings : public QDialog
{
    Q_OBJECT

public:
    explicit BuddySettings(QWidget *parent = nullptr, CmdFacade *lib = nullptr);
    ~BuddySettings();

private slots:
    void on_pushButton_close_clicked();

    void on_pushButton_remove_clicked();

    void on_pushButton_edit_clicked();

    void on_pushButton_add_clicked();

    void on_tableView_clicked(const QModelIndex &index);

    void on_BuddyEntryChanged();

private:
    Ui::BuddySettings *ui;
    CmdFacade* m_cmdFacade;
    buddydialog* m_buddyDialog = nullptr;
    QList<s_buddy> m_BuddyList;
    s_buddy m_buddyEdit;
    BuddyModel *m_buddyModel;
};

#endif // BUDDYSETTINGS_H
