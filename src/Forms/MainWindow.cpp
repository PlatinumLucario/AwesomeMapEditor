//////////////////////////////////////////////////////////////////////////////////
//
//
//                     d88b         888b           d888  888888888888
//                    d8888b        8888b         d8888  888
//                   d88''88b       888'8b       d8'888  888
//                  d88'  '88b      888 '8b     d8' 888  8888888
//                 d88Y8888Y88b     888  '8b   d8'  888  888
//                d88""""""""88b    888   '8b d8'   888  888
//               d88'        '88b   888    '888'    888  888
//              d88'          '88b  888     '8'     888  888888888888
//
//
// AwesomeMapEditor: A map editor for GBA Pokémon games.
// Copyright (C) 2016 Diegoisawesome, Pokedude
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
//////////////////////////////////////////////////////////////////////////////////


#include <AME/System/LoadedData.hpp>
#include <AME/System/Configuration.hpp>
#include <AME/System/Settings.hpp>
#include <AME/Widgets/Misc/Messages.hpp>
#include <AME/Widgets/OpenGL/AMEMapView.h>
#include <AME/Forms/MainWindow.h>
#include <AME/Forms/ErrorWindow.h>
#include "ui_MainWindow.h"
#include <QFileDialog>
#include <QtEvents>


namespace ame
{
    ///////////////////////////////////////////////////////////
    // Function type:  Constructor
    // Contributors:   Pokedude
    // Last edit by:   Pokedude
    // Date of edit:   6/2/2016
    // Comment:
    //
    // Added a function call to set the GUI up.
    //
    ///////////////////////////////////////////////////////////
    MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
    {
        ui->setupUi(this);
        m_lastOpenedMap = NULL;

        connect(ui->openGLWidget_5, SIGNAL(onMouseClick(QMouseEvent*)), this, SLOT(on_entity_mouseClick(QMouseEvent*)));

        statusLabel = new QLabel(tr("No ROM loaded."));
        ui->statusBar->addWidget(statusLabel);

        statusLabelCredit = new QLabel(tr("Created by ") + "<a href=\"http://domoreaweso.me/\">DoMoreAwesome</a>");
        statusLabelCredit->setTextFormat(Qt::RichText);
        statusLabelCredit->setTextInteractionFlags(Qt::TextBrowserInteraction);
        statusLabelCredit->setOpenExternalLinks(true);
        ui->statusBar->addPermanentWidget(statusLabelCredit);

        QMenu *mapSortOrderMenu = new QMenu();
        QActionGroup *mapSortOrderActionGroup = new QActionGroup(this);

        mapSortOrderMenu->addAction(ui->actionSort_by_Name);
        mapSortOrderMenu->addAction(ui->actionSort_by_Bank);
        mapSortOrderMenu->addAction(ui->actionSort_by_Layout);
        mapSortOrderMenu->addAction(ui->actionSort_by_Tileset);

        mapSortOrderActionGroup->addAction(ui->actionSort_by_Name);
        mapSortOrderActionGroup->addAction(ui->actionSort_by_Bank);
        mapSortOrderActionGroup->addAction(ui->actionSort_by_Layout);
        mapSortOrderActionGroup->addAction(ui->actionSort_by_Tileset);
        ui->tbMapSortOrder->setMenu(mapSortOrderMenu);

        connect(ui->tbMapSortOrder, SIGNAL(triggered(QAction*)), this, SLOT(on_MapSortOrder_changed(QAction*)));

        QActionGroup *mapToolbarActionGroup = new QActionGroup(this);
        mapToolbarActionGroup->addAction(ui->actionMouse);
        ui->btnMapMouse->setDefaultAction(ui->actionMouse);
        mapToolbarActionGroup->addAction(ui->actionPencil);
        ui->btnMapPencil->setDefaultAction(ui->actionPencil);
        mapToolbarActionGroup->addAction(ui->actionEyedropper);
        ui->btnMapEyedropper->setDefaultAction(ui->actionEyedropper);
        mapToolbarActionGroup->addAction(ui->actionFill);
        ui->btnMapFill->setDefaultAction(ui->actionFill);
        mapToolbarActionGroup->addAction(ui->actionFill_All);
        ui->btnMapFillAll->setDefaultAction(ui->actionFill_All);

        ui->btnMapGrid->setDefaultAction(ui->action_Show_Grid);
        ui->btnEntitiesGrid->setDefaultAction(ui->action_Show_Grid);

        if (!Settings::parse())
            return;         // TODO: create default config file if none exists

        disableBeforeROMLoad();

        if (SETTINGS(RecentFiles).count() > 0)
            loadRecentlyUsedFiles();
        else
            ui->actionRecent_Files->setEnabled(false);

        QAction* sortOrder = ui->tbMapSortOrder->menu()->actions()[SETTINGS(MapSortOrder)];
        ui->tbMapSortOrder->setIcon(sortOrder->icon());
        sortOrder->setChecked(true);

        m_proxyModel = new QFilterChildrenProxyModel(this);
        ui->treeView->setModel(m_proxyModel);
    }


    ///////////////////////////////////////////////////////////
    // Function type:  Destructor
    // Contributors:   Pokedude
    // Last edit by:   Pokedude
    // Date of edit:   6/2/2016
    // Comment:
    //
    // Added a call to delete the GUI.
    //
    ///////////////////////////////////////////////////////////
    MainWindow::~MainWindow()
    {
        delete ui;
    }



    ///////////////////////////////////////////////////////////
    // Function type:  I/O
    // Contributors:   Pokedude, Diegoisawesome
    // Last edit by:   Diegoisawesome
    // Date of edit:   7/5/2016
    //
    ///////////////////////////////////////////////////////////
    bool MainWindow::openRomDialog()
    {
        QString file = QFileDialog::getOpenFileName(
                    this,
                    tr("Open ROM file"),
                    SETTINGS(LastPath),
                    tr("ROMs (*.gba *.bin)")
        );

        // Determines whether the dialog was successful
        if (!file.isNull() && !file.isEmpty())
        {
            QFileInfo *info = new QFileInfo(file);
            CHANGESETTING(LastPath, info->absolutePath());
            Settings::write();
            return loadROM(file);
        }

        // Dialog cancelled
        return false;
    }

    ///////////////////////////////////////////////////////////
    // Function type:  I/O
    // Contributors:   Pokedude
    // Last edit by:   Pokedude
    // Date of edit:   6/16/2016
    //
    ///////////////////////////////////////////////////////////
    void MainWindow::clearBeforeLoading()
    {
        // First clears the tree-view
        //ui->treeView->collapseAll();
        //ui->treeView->clear();

        // Clears all the OpenGL widgets
        ui->openGLWidget->freeGL();
        ui->openGLWidget_2->freeGL();
        ui->openGLWidget_3->freeGL();
        ui->openGLWidget_5->freeGL();

        // Sets the tab index to the map-index
        ui->tabWidget->setCurrentIndex(0);
    }

    ///////////////////////////////////////////////////////////
    // Function type:  I/O
    // Contributors:   Diegoisawesome
    // Last edit by:   Diegoisawesome
    // Date of edit:   7/5/2016
    //
    ///////////////////////////////////////////////////////////
    void MainWindow::loadRecentlyUsedFiles()
    {
        QList<QString> recentFiles = SETTINGS(RecentFiles);
        ui->actionRecent_Files->setEnabled(true);
        QMenu *recentFilesMenu = new QMenu;
        for(int i = 0; i < recentFiles.count(); i++)
        {
            QAction *fileAction = new QAction(recentFilesMenu);
            fileAction->setText(QDir::toNativeSeparators(recentFiles[i]));
            fileAction->setData(recentFiles[i]);
            recentFilesMenu->addAction(fileAction);
            connect(fileAction, SIGNAL(triggered()), this, SLOT(on_RecentFile_triggered()));
        }
        recentFilesMenu->addSeparator();
        recentFilesMenu->addAction(ui->actionClearRecentFiles);
        ui->actionRecent_Files->setMenu(recentFilesMenu);
    }

    ///////////////////////////////////////////////////////////
    // Function type:  I/O
    // Contributors:   Pokedude, Diegoisawesome
    // Last edit by:   Diegoisawesome
    // Date of edit:   7/5/2016
    //
    ///////////////////////////////////////////////////////////
    bool MainWindow::loadROM(const QString &file)
    {
        // Add ROM file to recent files list
        QList<QString> recentFiles = SETTINGS(RecentFiles);

        int count = recentFiles.count();
        for (int i = 0; i < count; i++)
        {
            if (recentFiles[i] == file)
            {
                recentFiles.removeAt(i);
                i--;
                count--;
            }
        }

        recentFiles.prepend(file);
        if(recentFiles.count() > 10)
            recentFiles.removeLast();

        CHANGESETTING(RecentFiles, recentFiles);
        Settings::write();
        loadRecentlyUsedFiles();

        // Close a previous ROM and destroy objects
        if (m_Rom.info().isLoaded())
        {
            // Clears the ROM data
            m_Rom.clearCache();
            m_Rom.close();

            // Clears old data and UI
            clearAllMapData();
            clearBeforeLoading();
        }

        // Attempts to open the new ROM file
        if (!m_Rom.loadFromFile(file))
        {
            m_Rom.clearCache();
            m_Rom.close();

            Messages::showError(this, WND_ERROR_ROM);
            return false;
        }

        // Loading successful
        return true;
    }

    ///////////////////////////////////////////////////////////
    // Function type:  I/O
    // Contributors:   Pokedude, Diegoisawesome
    // Last edit by:   Diegoisawesome
    // Date of edit:   7/1/2016
    //
    ///////////////////////////////////////////////////////////
    void MainWindow::loadMapData()
    {
        /* TODO: Design form to show error messages */

        int result = loadAllMapData(m_Rom);
        if (result < 0)
        {
            ErrorWindow errorWindow(this);
            errorWindow.exec();
            return;
        }

        setWindowTitle(QString("Awesome Map Editor | %1").arg(m_Rom.info().name()));
        statusLabel->setText(tr("ROM %1 loaded in %2 ms.").arg(m_Rom.info().name(), QString::number(result)));

        setupAfterLoading();
        m_Rom.clearCache();
    }

    ///////////////////////////////////////////////////////////
    // Function type:  Event
    // Contributors:   Pokedude, Diegoisawesome
    // Last edit by:   Diegoisawesome
    // Date of edit:   7/2/2016
    //
    ///////////////////////////////////////////////////////////
    void MainWindow::setupAfterLoading()
    {
        QStandardItemModel *pokemonModel = new QStandardItemModel;
        for (unsigned i = 0; i < CONFIG(PokemonCount); i++)
        {
            QStandardItem *item = new QStandardItem;
            item->setText(dat_PokemonTable->names().at(i));
            item->setIcon(QIcon(QPixmap::fromImage(dat_PokemonTable->images().at(i))));
            pokemonModel->appendRow(item);
        }

        // Fills all wild Pokemon comboboxes with the names
        foreach (QComboBox *box, ui->tabWidget_3->findChildren<QComboBox *>())
            box->setModel(pokemonModel);

        // Sets the max Pokemon IDs within the spinboxes
        foreach (QSpinBox *box, ui->tabWidget_3->findChildren<QSpinBox *>(QRegularExpression("sbWild")))
            box->setRange(0, CONFIG(PokemonCount));

        QStandardItemModel *itemModel = new QStandardItemModel;
        itemModel->appendRow(new QStandardItem(tr("Coins")));
        for (unsigned i = 1; i < CONFIG(ItemCount); i++)
        {
            QStandardItem *item = new QStandardItem;
            item->setText(dat_ItemTable->names().at(i));
            itemModel->appendRow(item);
        }

        // Fills all item comboboxes with the names
        ui->cmbSignItem->setModel(itemModel);

        // Sets the max item IDs within the spinboxes
        ui->spnSignItem->setRange(0, CONFIG(ItemCount));


        // Updates the treeview
        updateTreeView();

        // Enables everything that should be on ROM load
        enableAfterROMLoad();
    }

    ///////////////////////////////////////////////////////////
    // Function type:  Event
    // Contributors:   Diegoisawesome
    // Last edit by:   Diegoisawesome
    // Date of edit:   7/2/2016
    //
    ///////////////////////////////////////////////////////////
    void MainWindow::disableBeforeROMLoad()
    {
        ui->centralWidget->setEnabled(false);
        ui->tabWidget->setEnabled(false);

        ui->action_Save_ROM->setEnabled(false);
        ui->action_Save_ROM_As->setEnabled(false);
        ui->action_Save_Map->setEnabled(false);
        ui->action_Reload_ROM->setEnabled(false);
        ui->action_Import->setEnabled(false);
        ui->action_Export->setEnabled(false);
        ui->action_Undo->setEnabled(false);
        ui->action_Redo->setEnabled(false);
        ui->action_World_Map_Editor->setEnabled(false);
        ui->action_Tileset_Editor->setEnabled(false);
        ui->action_Show_Grid->setEnabled(false);

        ui->cbMapTimeOfDay->setVisible(false);
        ui->cbEntitiesTimeOfDay->setVisible(false);
    }

    ///////////////////////////////////////////////////////////
    // Function type:  Event
    // Contributors:   Diegoisawesome
    // Last edit by:   Diegoisawesome
    // Date of edit:   7/2/2016
    //
    ///////////////////////////////////////////////////////////
    void MainWindow::enableAfterROMLoad()
    {
        ui->centralWidget->setEnabled(true);
        ui->action_Save_ROM->setEnabled(true);
        ui->action_Save_ROM_As->setEnabled(true);
        ui->action_Reload_ROM->setEnabled(true);
        ui->action_World_Map_Editor->setEnabled(true);
    }

    ///////////////////////////////////////////////////////////
    // Function type:  Event
    // Contributors:   Diegoisawesome
    // Last edit by:   Diegoisawesome
    // Date of edit:   7/2/2016
    //
    ///////////////////////////////////////////////////////////
    void MainWindow::enableAfterMapLoad()
    {
        ui->tabWidget->setEnabled(true);

        ui->action_Save_Map->setEnabled(true);
        ui->action_Import->setEnabled(true);
        ui->action_Export->setEnabled(true);
        ui->action_Tileset_Editor->setEnabled(true);
        ui->action_Show_Grid->setEnabled(true);
    }

    ///////////////////////////////////////////////////////////
    // Function type:  Event
    // Contributors:   Diegoisawesome
    // Last edit by:   Diegoisawesome
    // Date of edit:   7/2/2016
    //
    ///////////////////////////////////////////////////////////
    void MainWindow::updateTreeView()
    {
        // Fills the tree-view with all the maps
        //ui->treeView->collapseAll();
        ui->treeView->setUpdatesEnabled(false);

        // Creates icons for use in treeView
        QIcon mapFolderIcon;
        mapFolderIcon.addFile(QStringLiteral(":/icons/folder_closed_map.ico"), QSize(), QIcon::Normal, QIcon::Off);
        mapFolderIcon.addFile(QStringLiteral(":/icons/folder_map.ico"), QSize(), QIcon::Normal, QIcon::On);

        QIcon folderIcon;
        folderIcon.addFile(QStringLiteral(":/icons/folder_closed.ico"), QSize(), QIcon::Normal, QIcon::Off);

        QIcon mapIcon;
        mapIcon.addFile(QStringLiteral(":/icons/map.ico"), QSize(), QIcon::Normal, QIcon::Off);
        mapIcon.addFile(QStringLiteral(":/icons/image.ico"), QSize(), QIcon::Normal, QIcon::On);

        QStandardItemModel *standardModel = new QStandardItemModel;
        QStandardItem *root = standardModel->invisibleRootItem();

        switch(SETTINGS(MapSortOrder))
        {
            case MSO_Name:
            default:
            {
                // Adds every map name to the tree. Format: [<index>] <map name>
                int nameCount = CONFIG(MapNameCount);
                int nameTotal = CONFIG(MapNameTotal);
                for (int i = 0; i < nameCount; i++)
                {
                    QStandardItem *nameItem = new QStandardItem;
                    nameItem->setIcon(folderIcon);
                    nameItem->setEditable(false);

                    // Specifies the display name of the bank and adds it to the treeview
                    nameItem->setText('[' +
                                      QString("%1").arg(i + nameTotal - nameCount, 2 , 16, QChar('0')).toUpper() +
                                      "] " +
                                      dat_MapNameTable->names()[i]->name);
                    root->appendRow(nameItem);
                }
                int bankCount = dat_MapBankTable->banks().size();
                for (int i = 0; i < bankCount; i++)
                {
                    MapBank *bank = dat_MapBankTable->banks()[i];
                    // Adds all the bank's maps to the appropriate name
                    int mapCount = bank->maps().size();
                    for (int j = 0; j < mapCount; j++)
                    {
                        Map *map = bank->maps()[j];
                        int nameIndex = map->nameIndex() + nameCount - nameTotal;
                        QStandardItem *nameItem = root->child(nameIndex);
                        nameItem->setIcon(mapFolderIcon);

                        QStandardItem *mapItem = new QStandardItem();
                        mapItem->setIcon(mapIcon);
                        mapItem->setEditable(false);

                        mapItem->setText('[' +
                            QString("%1").arg(i, 2 , 16, QChar('0')).toUpper() +
                            ", " +
                            QString("%1").arg(j, 2 , 16, QChar('0')).toUpper() +
                            "] " +
                            dat_MapNameTable->names()[nameIndex]->name);

                        nameItem->appendRow(mapItem);

                        // Sets properties to identify map on click
                        QByteArray array;
                        array.append(i);
                        array.append(j);
                        mapItem->setData(array, Qt::UserRole);
                    }
                }
                break;
            }
            case MSO_Bank:
            {
                // Adds every bank and map to the tree. Format: [<bank>,<map>]
                int bankCount = dat_MapBankTable->banks().size();
                for (int i = 0; i < bankCount; i++)
                {
                    QStandardItem *bankItem = new QStandardItem;
                    MapBank *bank = dat_MapBankTable->banks()[i];
                    bankItem->setIcon(mapFolderIcon);
                    bankItem->setEditable(false);

                    // Adds all the bank's maps
                    int mapCount = bank->maps().size();
                    for (int j = 0; j < mapCount; j++)
                    {
                        QStandardItem *mapItem = new QStandardItem;
                        Map *map = bank->maps()[j];
                        mapItem->setIcon(mapIcon);
                        mapItem->setEditable(false);

                        mapItem->setText("[" +
                            QString("%1").arg(i, 2 , 16, QChar('0')).toUpper() +
                            ", " +
                            QString("%1").arg(j, 2 , 16, QChar('0')).toUpper() +
                            "] " +
                            dat_MapNameTable->names()[map->nameIndex() + CONFIG(MapNameCount) - CONFIG(MapNameTotal)]->name);

                        // Sets properties to identify map on click
                        QByteArray array;
                        array.append(i);
                        array.append(j);
                        mapItem->setData(array, Qt::UserRole);

                        // Adds the map to the bank
                        bankItem->appendRow(mapItem);
                    }

                    // Specifies the display name of the bank and adds it to the treeview
                    bankItem->setText('[' + QString("%1").arg(i, 2 , 16, QChar('0')).toUpper() + ']');
                    root->appendRow(bankItem);
                }
                break;
            }
            case MSO_Layout:
            {
                // Adds every layout index to the tree.
                int layoutTotal = dat_MapLayoutTable->count();
                for (int i = 1; i < layoutTotal; i++)
                {
                    QStandardItem *layoutItem = new QStandardItem;
                    layoutItem->setIcon(mapIcon);
                    layoutItem->setEditable(false);

                    // Specifies the display name of the bank and adds it to the treeview
                    layoutItem->setText('[' +
                                      QString("%1").arg(i, 4 , 16, QChar('0')).toUpper() +
                                      "] ");
                    layoutItem->setData(dat_MapLayoutTable->mapHeaders().at(i - 1)->offset(), Qt::UserRole);
                    root->appendRow(layoutItem);
                }
                int bankCount = dat_MapBankTable->banks().size();
                for (int i = 0; i < bankCount; i++)
                {
                    MapBank *bank = dat_MapBankTable->banks()[i];
                    // Adds all the bank's maps to the appropriate layout
                    int mapCount = bank->maps().size();
                    for (int j = 0; j < mapCount; j++)
                    {
                        Map *map = bank->maps()[j];
                        QStandardItem *layoutItem = root->child(map->layoutIndex() - 1);
                        layoutItem->setIcon(mapFolderIcon);

                        QStandardItem *mapItem = new QStandardItem();
                        mapItem->setIcon(mapIcon);
                        mapItem->setEditable(false);

                        mapItem->setText('[' +
                            QString("%1").arg(i, 2 , 16, QChar('0')).toUpper() +
                            ", " +
                            QString("%1").arg(j, 2 , 16, QChar('0')).toUpper() +
                            "] " +
                            dat_MapNameTable->names()[map->nameIndex() + CONFIG(MapNameCount) - CONFIG(MapNameTotal)]->name);

                        layoutItem->appendRow(mapItem);

                        // Sets properties to identify map on click
                        QByteArray array;
                        array.append(i);
                        array.append(j);
                        mapItem->setData(array, Qt::UserRole);
                    }
                }
                break;
            }
            case MSO_Tileset:
            {
                // Adds every tileset offset to the tree.
                int bankCount = dat_MapBankTable->banks().size();
                for (int i = 0; i < bankCount; i++)
                {
                    MapBank *bank = dat_MapBankTable->banks()[i];
                    // Adds all the bank's maps to the appropriate layout
                    int mapCount = bank->maps().size();
                    for (int j = 0; j < mapCount; j++)
                    {
                        Map *map = bank->maps()[j];
                        QModelIndexList primaryList;
                        if (root->rowCount() > 0)
                            primaryList = root->model()->match(root->child(0)->index(), Qt::UserRole, map->header().ptrPrimary());
                        QModelIndexList secondaryList;
                        if (root->rowCount() > 0)
                            secondaryList = root->model()->match(root->child(0)->index(), Qt::UserRole, map->header().ptrSecondary());

                        QStandardItem *primaryItem;
                        QStandardItem *secondaryItem;

                        if (primaryList.count() <= 0 || !primaryList.first().isValid())
                        {
                            primaryItem = new QStandardItem();
                            primaryItem->setEditable(false);
                            primaryItem->setText('[' +
                                              QString("%1").arg(map->header().ptrPrimary(), 8 , 16, QChar('0')).toUpper() +
                                              "] ");
                            primaryItem->setData(map->header().ptrPrimary(), Qt::UserRole);
                            root->appendRow(primaryItem);
                        }
                        else
                            primaryItem = static_cast<QStandardItemModel*>(root->model())->itemFromIndex(primaryList.first());

                        if (secondaryList.count() <= 0 || !secondaryList.first().isValid())
                        {
                            secondaryItem = new QStandardItem();
                            secondaryItem->setEditable(false);
                            secondaryItem->setText('[' +
                                              QString("%1").arg(map->header().ptrSecondary(), 8 , 16, QChar('0')).toUpper() +
                                              "] ");
                            secondaryItem->setData(map->header().ptrSecondary(), Qt::UserRole);
                            root->appendRow(secondaryItem);
                        }
                        else
                            secondaryItem = static_cast<QStandardItemModel*>(root->model())->itemFromIndex(secondaryList.first());

                        primaryItem->setIcon(mapFolderIcon);
                        secondaryItem->setIcon(mapFolderIcon);
\
                        QStandardItem *mapItem = new QStandardItem();
                        mapItem->setIcon(mapIcon);
                        mapItem->setEditable(false);

                        mapItem->setText('[' +
                            QString("%1").arg(i, 2 , 16, QChar('0')).toUpper() +
                            ", " +
                            QString("%1").arg(j, 2 , 16, QChar('0')).toUpper() +
                            "] " +
                            dat_MapNameTable->names()[map->nameIndex() + CONFIG(MapNameCount) - CONFIG(MapNameTotal)]->name);

                        // Sets properties to identify map on click
                        QByteArray array;
                        array.append(i);
                        array.append(j);
                        mapItem->setData(array, Qt::UserRole);

                        primaryItem->appendRow(mapItem);
                        secondaryItem->appendRow(mapItem->clone());
                    }
                }
                m_proxyModel->sort(0, Qt::AscendingOrder);
                break;
            }
        }
        m_proxyModel->setSourceModel(standardModel);

        // Repaint tree-view
        ui->treeView->setUpdatesEnabled(true);
        ui->treeView->header()->setDefaultSectionSize(600);
        ui->treeView->repaint();
    }

    ///////////////////////////////////////////////////////////
    // Function type:  Event
    // Contributors:   Pokedude
    // Last edit by:   Pokedude
    // Date of edit:   6/19/2016
    //
    ///////////////////////////////////////////////////////////
    void MainWindow::setupWildPokemon(Map *map)
    {
        ui->tabWidget_3->setCurrentIndex(0);
        ui->tabWidget_3->setTabEnabled(0, false);
        ui->tabWidget_3->setTabEnabled(1, false);
        ui->tabWidget_3->setTabEnabled(2, false);
        ui->tabWidget_3->setTabEnabled(3, false);


        if (map->wildpokeTable() == -1)
            return;

        WildPokemonSubTable *table = dat_WildPokemonTable->tables()[map->wildpokeTable()];
        WildPokemonArea &grass = table->encounter(EA_AREA_GRASS);
        WildPokemonArea &water = table->encounter(EA_AREA_WATER);
        WildPokemonArea &fish = table->encounter(EA_AREA_FISH);
        WildPokemonArea &rock = table->encounter(EA_AREA_ROCK);


        // Fills the encounter probabilities
        ui->sbWildGrassChance->setValue(grass.probability());
        ui->sbWildWaterChance->setValue(water.probability());
        ui->sbWildFishingChance->setValue(fish.probability());
        ui->sbWildOtherChance->setValue(rock.probability());
        ui->tabWidget_3->setCurrentIndex(0);

        // Fills the pokemon ID and min/max level
        for (int i = 0; i < grass.entries().size(); i++)
        {
            QString num = QString::number(i+1);
            WildPokemonEncounter *pkm = grass.entries().at(i);

            ui->tabWidget_3
                    ->findChild<QSpinBox *>(QString("sbWildGrass") + num)
                    ->setValue(pkm->id);

            ui->tabWidget_3
                    ->findChild<QSpinBox *>(QString("sbGrassMin") + num)
                    ->setValue(pkm->min);

            ui->tabWidget_3
                    ->findChild<QSpinBox *>(QString("sbGrassMax") + num)
                    ->setValue(pkm->max);
        }
        for (int i = 0; i < water.entries().size(); i++)
        {
            QString num = QString::number(i+1);
            WildPokemonEncounter *pkm = water.entries().at(i);

            ui->tabWidget_3
                    ->findChild<QSpinBox *>(QString("sbWildWater") + num)
                    ->setValue(pkm->id);

            ui->tabWidget_3
                    ->findChild<QSpinBox *>(QString("sbWaterMin") + num)
                    ->setValue(pkm->min);

            ui->tabWidget_3
                    ->findChild<QSpinBox *>(QString("sbWaterMax") + num)
                    ->setValue(pkm->max);
        }
        for (int i = 0; i < fish.entries().size(); i++)
        {
            QString num = QString::number(i+1);
            WildPokemonEncounter *pkm = fish.entries().at(i);

            ui->tabWidget_3
                    ->findChild<QSpinBox *>(QString("sbWildFishing") + num)
                    ->setValue(pkm->id);

            ui->tabWidget_3
                    ->findChild<QSpinBox *>(QString("sbFishingMin") + num)
                    ->setValue(pkm->min);

            ui->tabWidget_3
                    ->findChild<QSpinBox *>(QString("sbFishingMax") + num)
                    ->setValue(pkm->max);
        }
        for (int i = 0; i < rock.entries().size(); i++)
        {
            QString num = QString::number(i+1);
            WildPokemonEncounter *pkm = rock.entries().at(i);

            ui->tabWidget_3
                    ->findChild<QSpinBox *>(QString("sbWildOther") + num)
                    ->setValue(pkm->id);

            ui->tabWidget_3
                    ->findChild<QSpinBox *>(QString("sbOtherMin") + num)
                    ->setValue(pkm->min);

            ui->tabWidget_3
                    ->findChild<QSpinBox *>(QString("sbOtherMax") + num)
                    ->setValue(pkm->max);
        }


        // Enables the tabs, depending on which wild-pokemon areas exist
        if (grass.entries().size() > 0)
            ui->tabWidget_3->setTabEnabled(0, true);
        if (water.entries().size() > 0)
            ui->tabWidget_3->setTabEnabled(1, true);
        if (fish.entries().size() > 0)
            ui->tabWidget_3->setTabEnabled(2, true);
        if (rock.entries().size() > 0)
            ui->tabWidget_3->setTabEnabled(3, true);
    }

    ///////////////////////////////////////////////////////////
    // Function type:  Event
    // Contributors:   Pokedude
    // Last edit by:   Pokedude
    // Date of edit:   7/2/2016
    //
    ///////////////////////////////////////////////////////////
    void MainWindow::setupHeader(Map *map)
    {
        // Loads all the map-specific data
        ui->header_ptr_footer->setValue(map->m_PtrHeader);
        ui->header_ptr_script->setValue(map->m_PtrScripts);
        ui->header_ptr_event->setValue(map->m_PtrEvents);
        ui->header_ptr_connex->setValue(map->m_PtrConnections);

        ui->header_mapname->setValue(map->m_NameIndex);
        ui->header_ftr_index->setValue(map->m_HeaderID);
        ui->header_music_index->setValue(map->m_MusicID);
        ui->cbVisibility->setCurrentIndex(map->m_DarknessType);
        ui->cbMapType->setCurrentIndex(map->m_MapType);
        ui->cbWeather->setCurrentIndex(map->m_WeatherType);
        ui->cbBattleBG->setCurrentIndex(map->m_BattleType);
        ui->header_check_run->setChecked(map->m_MapType != 5 && map->m_MapType != 8);
        ui->header_check_bike->setChecked(map->m_MapType != 5 && map->m_MapType != 8 && map->m_MapType != 9);
        ui->header_check_bike->setChecked(map->m_MapType != 5 && map->m_MapType != 8 && map->m_MapType != 9);
        ui->header_check_showname->setChecked(map->m_LabelType == 1 || map->m_LabelType == 6 || map->m_LabelType == 13);
        ui->header_raw_data->setData(map->rawData());
        ui->header_group_raw->setTitle(QString("Raw Data @ 0x") + QString::number(map->m_Offset, 16));


        // Loads all the footer-specific data
        MapHeader &header = map->header();
        ui->header_ftr_ptr_border->setValue(header.m_PtrBorder);
        ui->header_ftr_ptr_data->setValue(header.m_PtrBlocks);
        ui->header_ftr_ptr_global->setValue(header.m_PtrPrimary);
        ui->header_ftr_ptr_local->setValue(header.m_PtrSecondary);
        ui->header_map_width->setText(QString::number(header.size().width()));
        ui->header_map_height->setText(QString::number(header.size().height()));
        ui->header_border_width->setText(QString::number(header.border().width()));
        ui->header_border_height->setText(QString::number(header.border().height()));
        ui->header_ftr_raw_data->setData(header.rawData());
        ui->header_ftr_group_raw->setTitle(QString("Raw Data @ 0x") + QString::number(header.m_Offset, 16));
    }


    ///////////////////////////////////////////////////////////
    // Function type:  Slot
    // Contributors:   Pokedude
    // Last edit by:   Pokedude
    // Date of edit:   6/16/2016
    //
    ///////////////////////////////////////////////////////////
    void MainWindow::on_action_Open_ROM_triggered()
    {
        if (openRomDialog())
            loadMapData();
    }

    ///////////////////////////////////////////////////////////
    // Function type:  Slot
    // Contributors:   Pokedude
    // Last edit by:   Pokedude
    // Date of edit:   6/19/2016
    //
    ///////////////////////////////////////////////////////////
    void MainWindow::on_sldWildGrassChance_valueChanged(int value)
    {
        ui->lblWildGrassChance->setText(QString::number((int)((value / 255.0)*100)) + QString("%"));
    }

    ///////////////////////////////////////////////////////////
    // Function type:  Slot
    // Contributors:   Pokedude
    // Last edit by:   Pokedude
    // Date of edit:   6/19/2016
    //
    ///////////////////////////////////////////////////////////
    void MainWindow::on_sldWildWaterChance_valueChanged(int value)
    {
        ui->lblWildWaterChance->setText(QString::number((int)((value / 255.0)*100)) + QString("%"));
    }

    ///////////////////////////////////////////////////////////
    // Function type:  Slot
    // Contributors:   Pokedude
    // Last edit by:   Pokedude
    // Date of edit:   6/19/2016
    //
    ///////////////////////////////////////////////////////////
    void MainWindow::on_sldWildFishingChance_valueChanged(int value)
    {
        ui->lblWildFishingChance->setText(QString::number((int)((value / 255.0)*100)) + QString("%"));
    }

    ///////////////////////////////////////////////////////////
    // Function type:  Slot
    // Contributors:   Pokedude
    // Last edit by:   Pokedude
    // Date of edit:   6/19/2016
    //
    ///////////////////////////////////////////////////////////
    void MainWindow::on_sldWildOtherChance_valueChanged(int value)
    {
        ui->lblWildOtherChance->setText(QString::number((int)((value / 255.0)*100)) + QString("%"));
    }


    ///////////////////////////////////////////////////////////
    // Function type:  Slot
    // Contributors:   Pokedude, Diegoisawesome
    // Last edit by:   Pokedude
    // Date of edit:   7/5/2016
    //
    ///////////////////////////////////////////////////////////
    void MainWindow::on_treeView_doubleClicked(const QModelIndex &index)
    {
        if (!index.parent().isValid())
        {
            if (index.child(0, 0).isValid())
                return;

            if (SETTINGS(MapSortOrder) == MSO_Layout)
            {
                // Switch icon
                if(m_lastOpenedMap != NULL)
                {
                    ui->treeView->setExpanded(*m_lastOpenedMap, false);
                    delete m_lastOpenedMap;
                }

                ui->treeView->setExpanded(index, true);
                m_lastOpenedMap = new QModelIndex(index);

                // Fills the map tab
                UInt32 offset = ui->treeView->model()->data(index, Qt::UserRole).toUInt();
                if (offset == 0)
                {
                    // Map layout is NULL pointer, abort
                    Messages::showMessage(this, "Layout says: \"I don't exist! *cries*\"");
                    return;
                }

                MapHeader header;
                header.read(m_Rom, offset);
                if (header.primary()->image()->raw().isEmpty() ||
                    header.primary()->image()->raw().isEmpty())
                {
                    // Tilesets invalid, abort
                    Messages::showMessage(this, "Layout says: \"I don't want to load tilesets!\"");
                    return;
                }


                ui->openGLWidget->freeGL();
                ui->openGLWidget_2->freeGL();
                ui->openGLWidget_3->freeGL();
                ui->openGLWidget_2->setLayout(header);
                ui->openGLWidget_2->makeGL();
                ui->openGLWidget_2->update();
                ui->openGLWidget_3->setMapView(ui->openGLWidget_2);
                ui->openGLWidget_3->update();
                ui->openGLWidget->setMapView(ui->openGLWidget_2, true);
                ui->openGLWidget->update();

                // Sets up the header, invisibles
                ui->tabWidget->setTabEnabled(1, false);
                ui->tabWidget->setTabEnabled(2, false);
                //setupHeader(currentMap);
                enableAfterMapLoad();
                return;
            }
            else
            {
                return;
            }
        }

        // Switch icon
        if(m_lastOpenedMap != NULL)
        {
            ui->treeView->setExpanded(*m_lastOpenedMap, false);
            delete m_lastOpenedMap;
        }

        ui->treeView->setExpanded(index, true);
        m_lastOpenedMap = new QModelIndex(index);

        ui->tabWidget->setTabEnabled(1, true);
        ui->tabWidget->setTabEnabled(2, true);
        enableAfterMapLoad();

        // Counts how long it takes to load a map
        QTime stopWatch;
        stopWatch.start();

        // Clears all the OpenGL widgets
        ui->openGLWidget->freeGL();
        ui->openGLWidget_2->freeGL();
        ui->openGLWidget_3->freeGL();
        ui->openGLWidget_5->freeGL();


        // Retrieves the new map from the stored property
        QByteArray data = ui->treeView->model()->data(index, Qt::UserRole).toByteArray();
        Map *currentMap = dat_MapBankTable->banks()[data.at(0)]->maps()[data.at(1)];

        // Fills all the OpenGL widgets
        ui->openGLWidget_2->setMap(m_Rom, currentMap);
        ui->openGLWidget_2->makeGL();
        ui->openGLWidget_2->update();
        ui->openGLWidget_3->setMapView(ui->openGLWidget_2);
        ui->openGLWidget_3->update();
        ui->openGLWidget->setMapView(ui->openGLWidget_2);
        ui->openGLWidget->update();
        ui->openGLWidget_5->setMapView(ui->openGLWidget_2);
        ui->openGLWidget_5->setEntities(currentMap);
        m_CurrentMap = currentMap;

        // Fills the wild-pokemon tab
        setupWildPokemon(currentMap);
        
        // Fills the header tab
        setupHeader(currentMap);
        ui->cmbEntityTypeSelector->setCurrentIndex(0);
        on_cmbEntityTypeSelector_currentIndexChanged(0);

        // FIX: Scroll to main map
        QTimer::singleShot
        (
            10, Qt::PreciseTimer, [this] ()
            {
                QPoint scrollPos = ui->openGLWidget_2->mainPos();
                QSize size = ui->openGLWidget_2->mainMap()->header().size();
                ui->scrollArea->horizontalScrollBar()->setValue(scrollPos.x() - (ui->scrollArea->viewport()->width() -
                                                                 size.width() * 16) / 2);
                ui->scrollArea->verticalScrollBar()->setValue(scrollPos.y() - (ui->scrollArea->viewport()->height() -
                                                               size.height() * 16) / 2);
                ui->scrollArea_5->horizontalScrollBar()->setValue(scrollPos.x() - (ui->scrollArea_5->viewport()->width() -
                                                                   size.width() * 16) / 2);
                ui->scrollArea_5->verticalScrollBar()->setValue(scrollPos.y() - (ui->scrollArea_5->viewport()->height() -
                                                                 size.height() * 16) / 2);
            }
        );

        setWindowTitle(QString("Awesome Map Editor | %1 | %2").arg(m_Rom.info().name(), ui->treeView->model()->data(index, Qt::DisplayRole).toString()));
        statusLabel->setText(tr("Map %1 loaded in %2 ms.").arg(ui->treeView->model()->data(index, Qt::DisplayRole).toString(), QString::number(stopWatch.elapsed())));
    }


    ///////////////////////////////////////////////////////////
    // Function type:  Slot
    // Contributors:   Diegoisawesome
    // Last edit by:   Diegoisawesome
    // Date of edit:   7/2/2016
    //
    ///////////////////////////////////////////////////////////
    void MainWindow::on_MapSortOrder_changed(QAction *action)
    {
        ui->tbMapSortOrder->setIcon(action->icon());
        QList<QAction*> items = ui->tbMapSortOrder->menu()->actions();
        int index = 0;
        for (int i = 0; i < items.count(); i++)
        {
            if(items[i] == action)
            {
                index = i;
                break;
            }
        }
        CHANGESETTING(MapSortOrder, static_cast<MapSortOrderType>(index));
        Settings::write();
        if(m_Rom.info().isLoaded())
            updateTreeView();
    }

    ///////////////////////////////////////////////////////////
    // Function type:  Slot
    // Contributors:   Pokedude
    // Last edit by:   Pokedude
    // Date of edit:   7/3/2016
    //
    ///////////////////////////////////////////////////////////
    void MainWindow::on_cmbEntityTypeSelector_currentIndexChanged(int index)
    {
        if (index == 0)
        {
            ui->spnEntityScroller->setMinimum(0);
            ui->spnEntityScroller->setMaximum(m_CurrentMap->entities().npcs().size()-1);
            ui->stckEntityEditor->setEnabled(m_CurrentMap->entities().npcs().size() > 0);

            if (m_CurrentMap->entities().npcs().size() > 0)
                on_spnEntityScroller_valueChanged(0);
            else
                ui->openGLWidget_5->setCurrentEntity(CurrentEntity());
        }
        else if (index == 1)
        {
            ui->spnEntityScroller->setMinimum(0);
            ui->spnEntityScroller->setMaximum(m_CurrentMap->entities().warps().size()-1);
            ui->stckEntityEditor->setEnabled(m_CurrentMap->entities().warps().size() > 0);

            if (m_CurrentMap->entities().warps().size() > 0)
                on_spnEntityScroller_valueChanged(0);
            else
                ui->openGLWidget_5->setCurrentEntity(CurrentEntity());
        }
        else if (index == 2)
        {
            ui->spnEntityScroller->setMinimum(0);
            ui->spnEntityScroller->setMaximum(m_CurrentMap->entities().triggers().size()-1);
            ui->stckEntityEditor->setEnabled(m_CurrentMap->entities().triggers().size() > 0);

            if (m_CurrentMap->entities().triggers().size() > 0)
                on_spnEntityScroller_valueChanged(0);
            else
                ui->openGLWidget_5->setCurrentEntity(CurrentEntity());
        }
        else
        {
            ui->spnEntityScroller->setMinimum(0);
            ui->spnEntityScroller->setMaximum(m_CurrentMap->entities().signs().size()-1);
            ui->stckEntityEditor->setEnabled(m_CurrentMap->entities().signs().size() > 0);

            if (m_CurrentMap->entities().signs().size() > 0)
                on_spnEntityScroller_valueChanged(0);
            else
                ui->openGLWidget_5->setCurrentEntity(CurrentEntity());
        }
    }

    ///////////////////////////////////////////////////////////
    // Function type:  Slot
    // Contributors:   Pokedude
    // Last edit by:   Pokedude
    // Date of edit:   7/3/2016
    //
    /////////////////////////////////////////////////////////
    void MainWindow::on_spnEntityScroller_valueChanged(int arg1)
    {
        if (arg1 < 0)
            return;


        QPoint startPos = ui->openGLWidget_2->mainPos();
        if (ui->cmbEntityTypeSelector->currentIndex() == 0 && m_CurrentMap->entities().npcs().size() > 0)
        {
            Npc *eventN = m_CurrentMap->entities().npcs()[arg1];
            ui->stckEntityEditor->setCurrentWidget(ui->pageNPCs);
            ui->npc_group_raw->setTitle(QString("Raw Data @ 0x") + QString::number(eventN->offset, 16));
            ui->npc_num->setValue(eventN->npcID);
            ui->npc_sprite->setValue(eventN->imageID);
            ui->npc_pos_x->setValue(eventN->positionX);
            ui->npc_pos_y->setValue(eventN->positionY);
            ui->spnNPCHeight->setValue(eventN->level);
            ui->npc_replacement->setValue(eventN->replacement);
            ui->spnNPCIdleAnim->setValue(eventN->behaviour);
            ui->npc_mov_x->setValue(eventN->moveRadiusX);
            ui->npc_mov_y->setValue(eventN->moveRadiusY);
            ui->npc_view_rad->setValue(eventN->viewRadius);
            ui->npc_trainer->setValue(eventN->property);
            ui->npc_script->setValue(eventN->ptrScript);
            ui->npc_flag->setValue(eventN->flag);
            ui->npc_raw_data->setData(eventN->rawData());
            ui->spnEntityScroller->setValue(arg1);
            startPos += QPoint(eventN->positionX*16, eventN->positionY*16);

            CurrentEntity entity;
            entity.absPos = startPos;
            entity.type = ET_Npc;
            entity.entity = eventN;
            entity.index = arg1;
            ui->openGLWidget_5->setCurrentEntity(entity);
        }
        else if (ui->cmbEntityTypeSelector->currentIndex() == 1 && m_CurrentMap->entities().warps().size() > 0)
        {
            Warp *eventW = m_CurrentMap->entities().warps()[arg1];
            ui->stckEntityEditor->setCurrentWidget(ui->pageWarps);
            ui->warp_group_raw->setTitle(QString("Raw Data @ 0x") + QString::number(eventW->offset, 16));
            ui->warp_pos_x->setValue(eventW->positionX);
            ui->warp_pos_y->setValue(eventW->positionY);
            ui->warp_number->setValue(eventW->warp);
            ui->warp_map->setValue(eventW->map);
            ui->warp_bank->setValue(eventW->bank);
            ui->spnWarpHeight->setValue(eventW->level);
            ui->warp_raw_data->setData(eventW->rawData());
            ui->spnEntityScroller->setValue(arg1);
            startPos += QPoint(eventW->positionX*16, eventW->positionY*16);

            CurrentEntity entity;
            entity.absPos = startPos;
            entity.type = ET_Warp;
            entity.entity = eventW;
            entity.index = arg1;
            ui->openGLWidget_5->setCurrentEntity(entity);
        }
        else if (ui->cmbEntityTypeSelector->currentIndex() == 2 && m_CurrentMap->entities().triggers().size() > 0)
        {
            Trigger *eventT = m_CurrentMap->entities().triggers()[arg1];
            ui->stckEntityEditor->setCurrentWidget(ui->pageTriggers);
            ui->trigger_group_raw->setTitle(QString("Raw Data @ 0x") + QString::number(eventT->offset, 16));
            ui->trigger_pos_x->setValue(eventT->positionX);
            ui->trigger_pos_y->setValue(eventT->positionY);
            ui->trigger_var->setValue(eventT->variable);
            ui->trigger_value->setValue(eventT->value);
            ui->trigger_script->setValue(eventT->ptrScript);
            ui->spnTriggerHeight->setValue(eventT->level);
            ui->trigger_raw_data->setData(eventT->rawData());
            ui->spnEntityScroller->setValue(arg1);
            startPos += QPoint(eventT->positionX*16, eventT->positionY*16);

            CurrentEntity entity;
            entity.absPos = startPos;
            entity.type = ET_Trigger;
            entity.entity = eventT;
            entity.index = arg1;
            ui->openGLWidget_5->setCurrentEntity(entity);
        }
        else if (m_CurrentMap->entities().signs().size() > 0)
        {
            Sign *eventS = m_CurrentMap->entities().signs()[arg1];
            ui->stckEntityEditor->setCurrentWidget(ui->pageSigns);
            ui->sign_group_raw->setTitle(QString("Raw Data @ 0x") + QString::number(eventS->offset, 16));
            ui->sign_pos_x->setValue(eventS->positionX);
            ui->sign_pos_y->setValue(eventS->positionY);
            ui->sign_script->setValue(eventS->ptrScript);
            ui->sign_raw_data->setData(eventS->rawData());
            ui->spnSignHeight->setValue(eventS->level);
            ui->spnSignType->setValue(static_cast<int>(eventS->type));
            ui->spnEntityScroller->setValue(arg1);
            startPos += QPoint(eventS->positionX*16, eventS->positionY*16);

            CurrentEntity entity;
            entity.absPos = startPos;
            entity.type = ET_Sign;
            entity.entity = eventS;
            entity.index = arg1;
            ui->openGLWidget_5->setCurrentEntity(entity);

            showCorrectSignType(eventS);
        }
    }

    ///////////////////////////////////////////////////////////
    // Function type:  Slot
    // Contributors:   Diegoisawesome
    // Last edit by:   Diegoisawesome
    // Date of edit:   7/2/2016
    //
    ///////////////////////////////////////////////////////////
    void MainWindow::on_cmbSignType_currentIndexChanged(int index)
    {
        Sign *eventS = m_CurrentMap->entities().signs()[ui->spnEntityScroller->value()];
        showCorrectSignType(eventS);
    }

    ///////////////////////////////////////////////////////////
    // Function type:  Slot
    // Contributors:   Pokedude
    // Last edit by:   Pokedude
    // Date of edit:   7/3/2016
    //
    ///////////////////////////////////////////////////////////
    void MainWindow::on_entity_mouseClick(QMouseEvent *event)
    {
        // Map coordinates to block-coordinates
        int blockX = (event->pos().x()-ui->openGLWidget_2->mainPos().x()) / 16;
        int blockY = (event->pos().y()-ui->openGLWidget_2->mainPos().y()) / 16;
        int indexN, indexW, indexT, indexS;

        Npc *eventN = nullptr;
        Warp *eventW = nullptr;
        Trigger *eventT = nullptr;
        Sign *eventS = nullptr;

        // Find the NPC entity at that position
        for (int i = 0; i < m_CurrentMap->entities().npcs().size(); i++)
        {
            Npc *npc = m_CurrentMap->entities().npcs()[i];
            if (npc->positionX == blockX && npc->positionY == blockY)
            {
                eventN = npc;
                indexN = i;
                break;
            }
        }
        if (eventN != NULL)
        {
            // Load NPC properties
            ui->stckEntityEditor->setCurrentWidget(ui->pageNPCs);
            ui->npc_group_raw->setTitle(QString("Raw Data @ 0x") + QString::number(eventN->offset, 16));
            ui->npc_num->setValue(eventN->npcID);
            ui->npc_sprite->setValue(eventN->imageID);
            ui->npc_pos_x->setValue(eventN->positionX);
            ui->npc_pos_y->setValue(eventN->positionY);
            ui->spnNPCHeight->setValue(eventN->level);
            ui->npc_replacement->setValue(eventN->replacement);
            ui->spnNPCIdleAnim->setValue(eventN->behaviour);
            ui->npc_mov_x->setValue(eventN->moveRadiusX);
            ui->npc_mov_y->setValue(eventN->moveRadiusY);
            ui->npc_view_rad->setValue(eventN->viewRadius);
            ui->npc_trainer->setValue(eventN->property);
            ui->npc_script->setValue(eventN->ptrScript);
            ui->npc_flag->setValue(eventN->flag);
            ui->npc_raw_data->setData(eventN->rawData());
            ui->spnEntityScroller->setValue(indexN);

            CurrentEntity entity;
            entity.absPos.setX((event->pos().x()/16)*16);
            entity.absPos.setY((event->pos().y()/16)*16);
            entity.type = ET_Npc;
            entity.entity = eventN;
            entity.index = indexN;
            ui->openGLWidget_5->setCurrentEntity(entity);

            return;
        }

        // Find the warp entity at that position
        for (int i = 0; i < m_CurrentMap->entities().warps().size(); i++)
        {
            Warp *warp = m_CurrentMap->entities().warps()[i];
            if (warp->positionX == blockX && warp->positionY == blockY)
            {
                eventW = warp;
                indexW = i;
                break;
            }
        }
        if (eventW != NULL)
        {
            // Load warp properties
            ui->stckEntityEditor->setCurrentWidget(ui->pageWarps);
            ui->warp_group_raw->setTitle(QString("Raw Data @ 0x") + QString::number(eventW->offset, 16));
            ui->warp_pos_x->setValue(eventW->positionX);
            ui->warp_pos_y->setValue(eventW->positionY);
            ui->warp_number->setValue(eventW->warp);
            ui->warp_map->setValue(eventW->map);
            ui->warp_bank->setValue(eventW->bank);
            ui->spnWarpHeight->setValue(eventW->level);
            ui->warp_raw_data->setData(eventW->rawData());
            ui->spnEntityScroller->setValue(indexW);

            CurrentEntity entity;
            entity.absPos.setX((event->pos().x()/16)*16);
            entity.absPos.setY((event->pos().y()/16)*16);
            entity.type = ET_Warp;
            entity.entity = eventW;
            entity.index = indexW;
            ui->openGLWidget_5->setCurrentEntity(entity);

            return;
        }

        // Find the trigger entity at that position
        for (int i = 0; i < m_CurrentMap->entities().triggers().size(); i++)
        {
            Trigger *trigger = m_CurrentMap->entities().triggers()[i];
            if (trigger->positionX == blockX && trigger->positionY == blockY)
            {
                eventT = trigger;
                indexT = i;
                break;
            }
        }
        if (eventT != NULL)
        {
            // Load trigger properties
            ui->stckEntityEditor->setCurrentWidget(ui->pageTriggers);
            ui->trigger_group_raw->setTitle(QString("Raw Data @ 0x") + QString::number(eventT->offset, 16));
            ui->trigger_pos_x->setValue(eventT->positionX);
            ui->trigger_pos_y->setValue(eventT->positionY);
            ui->trigger_var->setValue(eventT->variable);
            ui->trigger_value->setValue(eventT->value);
            ui->trigger_script->setValue(eventT->ptrScript);
            ui->spnTriggerHeight->setValue(eventT->level);
            ui->trigger_raw_data->setData(eventT->rawData());
            ui->spnEntityScroller->setValue(indexT);

            CurrentEntity entity;
            entity.absPos.setX((event->pos().x()/16)*16);
            entity.absPos.setY((event->pos().y()/16)*16);
            entity.type = ET_Trigger;
            entity.entity = eventT;
            entity.index = indexT;
            ui->openGLWidget_5->setCurrentEntity(entity);

            return;
        }

        // Find the sign entity at that position
        for (int i = 0; i < m_CurrentMap->entities().signs().size(); i++)
        {
            Sign *sign = m_CurrentMap->entities().signs()[i];
            if (sign->positionX == blockX && sign->positionY == blockY)
            {
                eventS = sign;
                indexS = i;
                break;
            }
        }
        if (eventS != NULL)
        {
            // Load sign properties
            ui->stckEntityEditor->setCurrentWidget(ui->pageSigns);
            ui->sign_group_raw->setTitle(QString("Raw Data @ 0x") + QString::number(eventS->offset, 16));
            ui->sign_pos_x->setValue(eventS->positionX);
            ui->sign_pos_y->setValue(eventS->positionY);
            ui->sign_script->setValue(eventS->ptrScript);
            ui->sign_raw_data->setData(eventS->rawData());
            ui->spnSignHeight->setValue(eventS->level);
            ui->spnSignType->setValue(static_cast<int>(eventS->type));
            ui->spnEntityScroller->setValue(indexS);

            CurrentEntity entity;
            entity.absPos.setX((event->pos().x()/16)*16);
            entity.absPos.setY((event->pos().y()/16)*16);
            entity.type = ET_Sign;
            entity.entity = eventS;
            entity.index = indexS;
            ui->openGLWidget_5->setCurrentEntity(entity);

            showCorrectSignType(eventS);

            return;
        }
    }

    ///////////////////////////////////////////////////////////
    // Function type:  Slot
    // Contributors:   Pokedude
    // Last edit by:   Pokedude
    // Date of edit:   7/3/2016
    //
    ///////////////////////////////////////////////////////////
    void MainWindow::on_tabWidget_currentChanged(int index)
    {
        QPoint scrollPos = ui->openGLWidget_2->mainPos();

        QSize size;
        if(ui->openGLWidget_2->mainMap() != NULL)
            size = ui->openGLWidget_2->mainMap()->header().size();
        else
            size = ui->openGLWidget_2->layoutHeader()->size();

        if (index == 0)
        {
            ui->scrollArea->horizontalScrollBar()->setValue(scrollPos.x() - (ui->scrollArea->viewport()->width() -
                                                             size.width() * 16) / 2);
            ui->scrollArea->verticalScrollBar()->setValue(scrollPos.y() - (ui->scrollArea->viewport()->height() -
                                                           size.height() * 16) / 2);
        }
        else if (index == 1)
        {
            ui->scrollArea_5->horizontalScrollBar()->setValue(scrollPos.x() - (ui->scrollArea_5->viewport()->width() -
                                                               size.width() * 16) / 2);
            ui->scrollArea_5->verticalScrollBar()->setValue(scrollPos.y() - (ui->scrollArea_5->viewport()->height() -
                                                             size.height() * 16) / 2);
        }
    }


    ///////////////////////////////////////////////////////////
    // Function type:  Event
    // Contributors:   Pokedude, Diegoisawesome
    // Last edit by:   Diegoisawesome
    // Date of edit:   7/2/2016
    //
    ///////////////////////////////////////////////////////////
    void MainWindow::showCorrectSignType(Sign *sign)
    {
        if (sign->type <= ST_ScriptLeft)
        {
            ui->ctrSignScript->setVisible(true);
            ui->ctrSignItem->setVisible(false);
            ui->ctrSignBase->setVisible(false);
            ui->sign_script->setValue(sign->ptrScript);
        }
        else if (sign->type == ST_SecretBase)
        {
            ui->ctrSignScript->setVisible(false);
            ui->ctrSignItem->setVisible(false);
            ui->ctrSignBase->setVisible(true);
            ui->sign_base_id->setValue(sign->baseID);
        }
        else
        {
            ui->ctrSignScript->setVisible(false);
            ui->ctrSignItem->setVisible(true);
            ui->ctrSignBase->setVisible(false);
            ui->spnSignItem->setValue(sign->item);
            ui->sign_item_hidden->setValue(sign->hiddenID);
            ui->sign_item_amount->setValue(sign->amount);
            ui->chkSignItemExact->setChecked(sign->exactRequired);
        }
    }

    ///////////////////////////////////////////////////////////
    // Function type:  Virtual
    // Contributors:   Pokedude
    // Last edit by:   Pokedude
    // Date of edit:   6/20/2016
    //
    ///////////////////////////////////////////////////////////
    void MainWindow::closeEvent(QCloseEvent *event)
    {
        // Destroys OpenGL objects
        delete ui->openGLWidget;
        delete ui->openGLWidget_2;
        delete ui->openGLWidget_3;
        delete ui->openGLWidget_5;

        ui->openGLWidget = NULL;
        ui->openGLWidget_2 = NULL;
        ui->openGLWidget_3 = NULL;
        ui->openGLWidget_5 = NULL;

        // Destroys the window
        QMainWindow::closeEvent(event);
    }

    ///////////////////////////////////////////////////////////
    // Function type:  Slot
    // Contributors:   Diegoisawesome
    // Last edit by:   Diegoisawesome
    // Date of edit:   7/4/2016
    //
    ///////////////////////////////////////////////////////////
    void MainWindow::on_lineEdit_textChanged(const QString &arg1)
    {
        m_proxyModel->setFilterRegExp(QRegExp(arg1, Qt::CaseInsensitive,
                                                                   QRegExp::FixedString));
    }

    ///////////////////////////////////////////////////////////
    // Function type:  Slot
    // Contributors:   Diegoisawesome
    // Last edit by:   Diegoisawesome
    // Date of edit:   7/5/2016
    //
    ///////////////////////////////////////////////////////////
    void MainWindow::on_RecentFile_triggered()
    {
        if(loadROM(((QAction*)sender())->data().toString()))
            loadMapData();
    }

    ///////////////////////////////////////////////////////////
    // Function type:  Slot
    // Contributors:   Diegoisawesome
    // Last edit by:   Diegoisawesome
    // Date of edit:   7/5/2016
    //
    ///////////////////////////////////////////////////////////
    void MainWindow::on_actionClearRecentFiles_triggered()
    {
        CHANGESETTING(RecentFiles, QList<QString>());
        Settings::write();
        ui->actionRecent_Files->setMenu(new QMenu);
        ui->actionRecent_Files->setEnabled(false);
    }


    ///////////////////////////////////////////////////////////
    // Function type:  Slot
    // Contributors:   Diegoisawesome
    // Last edit by:   Diegoisawesome
    // Date of edit:   7/5/2016
    //
    ///////////////////////////////////////////////////////////
    void MainWindow::on_actionExit_triggered()
    {

    }

    ///////////////////////////////////////////////////////////
    // Function type:  Slot
    // Contributors:   Pokedude
    // Last edit by:   Pokedude
    // Date of edit:   7/6/2016
    //
    ///////////////////////////////////////////////////////////
    void MainWindow::on_tabWidget_2_currentChanged(int index)
    {
        if (index == -1)
            return;

        if (index == 0)
        {
            ui->openGLWidget_2->setMovementMode(false);
            ui->openGLWidget_2->update();
        }
        else
        {
            ui->openGLWidget_2->setMovementMode(true);
            ui->openGLWidget_2->update();
        }
    }
}
