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

#ifndef EDITORWIDGED_H
#define EDITORWIDGED_H

#include <QDialog>
#include "lib/types.h"
#include "cmdfacade.h"
#include <QStringListModel>
#include <QStyledItemDelegate>
#include <QComboBox>
#include <QStandardItemModel>
#include <QSpinBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QHBoxLayout>
#include <qjsonobject.h>


// **************************** Editor widget ********************************************
class EditorWidget: public QWidget{
    Q_OBJECT
public:
    explicit EditorWidget(const QJsonObject *setting, QWidget *parent=nullptr );


    int currentIndex();
    QString currentText();
    int findText(QString text);
    void setCurrentIndex(int index);
    QString text() const;
    void setText(const QString &text);
    Qt::CheckState checkState() const;
    void setCheckState(Qt::CheckState state);
    void setValue(int value);
    void setMinimum(int min);
    void setMaximum(int max);
    int getValue();
    QString getText();

signals:
    void datachanged();

private:
    QCheckBox *checkBox;
    QLineEdit *lineBox;
    QComboBox *comboBox;
    QSpinBox *spinbox;
    const QJsonObject m_setting;
};


// ***************************Settings Delegate ***********************************************
class SettingsDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit SettingsDelegate(const QJsonObject *items, QJsonObject *editedItem, QObject *parent = 0);
    ~SettingsDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

private slots:
    void commitEditor();

private:
    const QJsonObject *m_items;
    QJsonObject *m_editeditem;
    QStringList keys;
    void updateData();
};



#endif // EDITORWIDGED_H
