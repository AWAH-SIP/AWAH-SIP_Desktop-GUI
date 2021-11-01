#include "buddysettings.h"
#include "ui_buddysettings.h"


BuddySettings::BuddySettings(QWidget *parent, CmdFacade *lib) :
    QDialog(parent),
    ui(new Ui::BuddySettings),
    m_cmdFacade(lib)
{
    connect(m_cmdFacade, &CmdFacade::buddyEntryChanged , this, &BuddySettings::on_BuddyEntryChanged);
    connect(m_cmdFacade, &CmdFacade::signalBuddyStatus , this, &BuddySettings::on_BuddyEntryChanged);
    ui->setupUi(this);
    QSettings settings("awah", "AWAHSipDesktopGUI");
    restoreGeometry(settings.value("BuddySettingsWindow/Geometry").toByteArray());
    m_BuddyList = m_cmdFacade->getBuddies();
    m_buddyModel = new BuddyModel(this, &m_BuddyList, m_cmdFacade);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setModel(m_buddyModel);
}

BuddySettings::~BuddySettings()
{
    if(m_buddyDialog != nullptr){
        delete m_buddyDialog;
    }
    delete ui;
}

void BuddySettings::on_pushButton_close_clicked()
{
    BuddySettings::close();
}

void BuddySettings::on_pushButton_remove_clicked()
{
    m_cmdFacade->removeBuddy(m_buddyEdit.uid);
}

void BuddySettings::on_pushButton_edit_clicked()
{
    if(m_buddyEdit.uid.isEmpty()){
        return;
    }
    s_buddy buddytoEdit = m_buddyEdit;
    m_buddyDialog = new buddydialog(m_cmdFacade,buddytoEdit,this);
    m_buddyDialog->setModal(true);
    m_buddyDialog->exec();
    if(!buddytoEdit.buddyUrl.isEmpty()){                                        // if cancel was pressed fields are empty
        m_cmdFacade->editBuddy(buddytoEdit.buddyUrl, buddytoEdit.Name, buddytoEdit.accUid, buddytoEdit.codec.toJSON(), buddytoEdit.uid);
        m_buddyEdit = buddytoEdit;
    }
}

void BuddySettings::on_pushButton_add_clicked()
{
    s_buddy newBuddy;
    m_buddyDialog = new buddydialog(m_cmdFacade,newBuddy,this);
    m_buddyDialog->setModal(true);
    m_buddyDialog->exec();
    if(!newBuddy.buddyUrl.isEmpty()){                                           // abused to check if cancel was pressed
        m_cmdFacade->addBuddy(newBuddy.buddyUrl, newBuddy.Name, newBuddy.accUid, newBuddy.codec.toJSON());
    }
}

void BuddySettings::on_BuddyEntryChanged()
{
    m_BuddyList = m_cmdFacade->getBuddies();
    m_buddyModel->refresh();
}

// *********************** Buddy Model needed to display the buddies ***************

BuddyModel::BuddyModel(QObject *parent,  QList<s_buddy> *buddies,CmdFacade *lib)
    : QAbstractTableModel(parent), m_BuddyList(buddies), m_cmdFacade(lib)
{

}


int BuddyModel::rowCount(const QModelIndex & /*parent*/) const
{
   return m_BuddyList->count();
}

int BuddyModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 5;
}

QVariant BuddyModel::data(const QModelIndex &index, int role) const
{
    auto account = m_cmdFacade->getAccountByUID(m_BuddyList->at(index.row()).accUid);
    if (role == Qt::DisplayRole || role == Qt::EditRole)
        switch (index.column()) {
            case 0:
                return m_BuddyList->at(index.row()).Name;
                break;
            case 1:
                return m_BuddyList->at(index.row()).buddyUrl;
                break;
            case 2:
                return m_BuddyList->at(index.row()).codec.displayName;
                break;
            case 3:
                return account->name;
                break;
            case 4:
            switch (m_BuddyList->at(index.row()).status) {
                case unknown:
                return QString("unknown");
                break;
            case online:
                return QString("online");
                break;
            case busy:
                return QString("busy");
                break;
            default:
                break;
            }
                break;
            default:
                return QVariant();
    }
    if (role == Qt::BackgroundColorRole){
        switch(m_BuddyList->at(index.row()).status) {
        case unknown:
            return QBrush(QColor(145, 157, 157));   // stone
            break;
        case online:
            return QBrush(QColor(113, 183, 144));   // green
            break;
        case busy:
            return QBrush(QColor(229, 85, 79));     // red
            break;
        }
    }
    return QVariant();
}

QVariant BuddyModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch(section) {
        case 0:
            return QString("Name");
            break;
        case 1:
            return QString("Buddy URI");
            break;
        case 2:
            return QString("Codec");
            break;
        case 3:
            return QString("Account");
            break;
        case 4:
            return QString("State");
            break;
        default:
            return QVariant();
        }
    }
    return QVariant();
}

void BuddyModel::refresh()
{
    emit dataChanged(index(0, 0),
                     index(rowCount(), columnCount()));  // update whole view
    emit layoutChanged();
}

void BuddySettings::on_tableView_clicked(const QModelIndex &index)
{
    m_buddyEdit = m_BuddyList.at(index.row());
}
