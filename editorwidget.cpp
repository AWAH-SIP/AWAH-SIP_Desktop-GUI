/*
 * Copyright (C) 2016 - 2021 Andy Weiss, Adi Hilber
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "editorwidget.h"



enum CustomRoles{
    SelectRole = Qt::UserRole
};


SettingsDelegate::SettingsDelegate(QObject *parent) :
        QStyledItemDelegate(parent)
{

}
SettingsDelegate::~SettingsDelegate()
{

}

void SettingsDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const{
    if(index.column() ==0){
    QStyledItemDelegate::paint(painter, option, index);
    }
}
QWidget *SettingsDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const{
    Q_UNUSED(option)

    if(index.column() ==1){
    s_setting tmp = m_map->values().at(index.row());
    EditorWidget *editor = new EditorWidget(tmp, parent);
    connect(editor, SIGNAL(datachanged()),
                     this, SLOT(commitEditor()));
    return editor;
    }
    return nullptr;
}
void SettingsDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const{
    EditorWidget *widget = static_cast<EditorWidget*>(editor);
    widget->blockSignals(true);
    widget->setText(index.data(Qt::DisplayRole).toString());
    Qt::CheckState state = static_cast<Qt::CheckState>(index.data(Qt::CheckStateRole).toInt());
    widget->setCheckState(state);

    if(!m_map->values().at(index.row()).enumlist.isEmpty()){                                // fill the combobox
        const int currentVal = index.data(Qt::EditRole).toInt();
        int idx = widget->findText(m_map->values().at(index.row()).enumlist.key(currentVal));    // search the key for the value in the enumlist
             widget->setCurrentIndex(idx);                     // set the index of the Combobox according to the key


    }
    else if (m_map->values().at(index.row()).value.userType()== QMetaType::Int || m_map->values().at(index.row()).value.userType()== QMetaType::UInt){       // fill the spinbox
        widget->setMinimum(m_map->values().at(index.row()).min);
        widget->setMaximum(m_map->values().at(index.row()).max);
        widget->setValue(m_map->values().at(index.row()).value.toInt());
    }
    else if (m_map->values().at(index.row()).value.userType()== QMetaType::QString){       // fill the Line edit
        widget->setText(m_map->values().at(index.row()).value.toString());
    }
    widget->blockSignals(false);
}


void SettingsDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const{
    EditorWidget *widget = static_cast<EditorWidget*>(editor);
    Q_UNUSED(model)
    s_setting editedsetting;

       if(!m_map->values().at(index.row()).enumlist.isEmpty()){
            int newvalue = m_map->values().at(index.row()).enumlist.value(widget->currentText()).toInt();
            editedsetting = m_map->values().at(index.row());                                        // get the setting struct
            editedsetting.value = newvalue;                                                         // edit it
            QString key =  m_map->keys().at(index.row());
            m_map->remove(key);
            m_map->insert(key,editedsetting);                                                       // and replace it!
       }
       else if (m_map->values().at(index.row()).value.userType() == QMetaType::Int || m_map->values().at(index.row()).value.userType()== QMetaType::UInt){
            editedsetting = m_map->values().at(index.row());
            editedsetting.value = widget->getValue();
            QString key =  m_map->keys().at(index.row());
            m_map->remove(key);
            m_map->insert(key,editedsetting);
       }
       else if (m_map->values().at(index.row()).value.userType() == QMetaType::QString){
            editedsetting = m_map->values().at(index.row());
            editedsetting.value = widget->getText();
            QString key =  m_map->keys().at(index.row());
            m_map->remove(key);
            m_map->insert(key,editedsetting);
       }

}
void SettingsDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const{
    Q_UNUSED(index)
    editor->setGeometry(option.rect);
}

QSize SettingsDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize s =  index.data(Qt::SizeHintRole).toSize();
    return s.isValid() ? s: QStyledItemDelegate::sizeHint(option, index);
}

void SettingsDelegate::commitEditor()
 {
     EditorWidget *editor = qobject_cast<EditorWidget *>(sender());
     emit commitData(editor);
 }



// ******************************************** Editor widget ********************************************

EditorWidget::EditorWidget(s_setting setting, QWidget *parent )
    : QWidget(parent),
      checkBox(new QCheckBox),
      lineBox(new QLineEdit),
      comboBox(new QComboBox),
      spinbox( new QSpinBox),
      m_setting(setting)
{

    QHBoxLayout *layout = new QHBoxLayout(this);
    if (!setting.enumlist.isEmpty()){
        comboBox->addItems(setting.enumlist.keys());
        layout->addWidget(comboBox);
        connect(comboBox,SIGNAL(currentIndexChanged(int)),
                this, SIGNAL(datachanged()));
    }
    else switch (setting.value.userType())
        {
            case QMetaType::Int:
            case QMetaType::UInt:
            case QMetaType::LongLong:
            case QMetaType::ULongLong:
                layout->addWidget(spinbox);
                connect(spinbox,SIGNAL(valueChanged(int)),
                        this, SIGNAL(datachanged()));
                break;
            case QMetaType::QString:
                layout->addWidget(lineBox);
                break;
            case QMetaType::Bool:
                layout->addWidget(checkBox);
                break;
            default:
                layout->addWidget(lineBox);
            break;
        }
}

int EditorWidget::currentIndex(){
    return comboBox->currentIndex();
}

QString EditorWidget::currentText(){
    return comboBox->currentText();
}

int EditorWidget::findText(QString text){
    return comboBox->findText(text);
}

void EditorWidget::setCurrentIndex(int index){
    comboBox->setCurrentIndex(index);
}

QString EditorWidget::text() const{
    return  lineBox->text();
}

void EditorWidget::setText(const QString &text){
    lineBox->setText(text);
}

Qt::CheckState EditorWidget::checkState() const{
    return checkBox->checkState();
}

void EditorWidget::setCheckState(Qt::CheckState state){
    checkBox->setCheckState(state);
}


void EditorWidget::setValue(int value){
    spinbox->setValue(value);
}

void EditorWidget::setMinimum(int min){
    spinbox->setMinimum(min);
}

void EditorWidget::setMaximum(int max){
    spinbox->setMaximum(max);
}

int EditorWidget::getValue(){
    return spinbox->value();
}

QString EditorWidget::getText(){
    return lineBox->text();
}
