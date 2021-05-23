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


SettingsDelegate::SettingsDelegate(const QJsonObject *items, QJsonObject *editedItem, QObject *parent) :
QStyledItemDelegate(parent), m_items(items), m_editeditem(editedItem)
{
    keys = m_items->keys();
    
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
        QJsonObject tmp = m_items->value(keys.at(index.row())).toObject();
        EditorWidget *editor = new EditorWidget(&tmp, parent);
        connect(editor, SIGNAL(datachanged()),this, SLOT(commitEditor()));
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
    
    if(m_items->value(keys.at(index.row())).toObject()["type"].toInt() == ENUM){                        // fill the combobox for enum types
        int currentVal = m_items->value(keys.at(index.row())).toObject()["value"].toInt();
        QStringList enums =m_items->value(keys.at(index.row())).toObject()["enumlist"].toObject().keys();
        QJsonObject enumlist = m_items->value(keys.at(index.row())).toObject()["enumlist"].toObject();
        QJsonObject::iterator i;
        int idx = -1;
        for (i = enumlist.begin(); i != enumlist.end(); ++i) {
            if (i.value().toInt() == currentVal){
                idx = widget->findText(i.key());                    // search the key for the value in the enumlist
            }
        }
        widget->setCurrentIndex(idx);                     // set the index of the Combobox according to the key
    }
    
    else if (m_items->value(keys.at(index.row())).toObject()["type"].toInt() == INTEGER){       // fill the spinbox for integer types
        widget->setMinimum(m_items->value(keys.at(index.row())).toObject()["min"].toInt());
        widget->setMaximum(m_items->value(keys.at(index.row())).toObject()["max"].toInt());
        widget->setValue(m_items->value(keys.at(index.row())).toObject()["value"].toInt());
    }
    
    else if (m_items->value(keys.at(index.row())).toObject()["type"].toInt() == STRING){       // fill the lineedit for string types
        widget->setText(m_items->value(keys.at(index.row())).toObject()["value"].toString());
    }
    widget->blockSignals(false);
}


void SettingsDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    EditorWidget *widget = static_cast<EditorWidget*>(editor);
    Q_UNUSED(model)
    if(m_items->value(keys.at(index.row())).toObject()["type"].toInt() == ENUM){
        QJsonObject enumlist = m_items->value(keys.at(index.row())).toObject()["enumlist"].toObject();      // search the new value in the enumlist
        int newvalue = enumlist[widget->currentText()].toInt();
        m_editeditem->insert(keys.at(index.row()), newvalue);
    }
    else if (m_items->value(keys.at(index.row())).toObject()["type"].toInt() == INTEGER){
        m_editeditem->insert(keys.at(index.row()), widget->getValue());
    }
    else if (m_items->value(keys.at(index.row())).toObject()["type"].toInt() == STRING){
        m_editeditem->insert(keys.at(index.row()), widget->getText());
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

EditorWidget::EditorWidget(const QJsonObject *setting, QWidget *parent )
: QWidget(parent),
checkBox(new QCheckBox),
lineBox(new QLineEdit),
comboBox(new QComboBox),
spinbox( new QSpinBox),
m_setting(*setting)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    switch (m_setting["type"].toInt() )
    {
        case ENUM:
            comboBox->addItems(m_setting["enumlist"].toObject().keys());
            layout->addWidget(comboBox);
            connect(comboBox,SIGNAL(currentIndexChanged(int)),
                    this, SIGNAL(datachanged()));
            break;
        case INTEGER:
            layout->addWidget(spinbox);
            connect(spinbox,SIGNAL(valueChanged(int)),
                    this, SIGNAL(datachanged()));
            break;
        case STRING:
            layout->addWidget(lineBox);
            connect(lineBox,SIGNAL(textChanged(QString)), this, SIGNAL(datachanged()));
            break;
        case BOOL_T:
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
