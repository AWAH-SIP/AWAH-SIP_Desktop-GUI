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


// **************************************************************************************
class EditorWidget: public QWidget{
    Q_OBJECT
public:
    explicit EditorWidget(s_setting setting, QWidget *parent=nullptr );


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
    s_setting m_setting;
};


// **************************************************************************************
class SettingsDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit SettingsDelegate(QObject *parent = 0);
    ~SettingsDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    inline void setMap(QMap<QString, s_setting >* map) {m_map = map; };

private slots:
    void commitEditor();

private:
    QMap<QString, s_setting >* m_map;
};



#endif // EDITORWIDGED_H
