import sys
import uno
import os
import shutil
from pathlib import Path
from datetime import datetime
from com.sun.star.task import ErrorCodeIOException

from constants import *

def connect2libreOffice():
    local_context = uno.getComponentContext()
    resolver = local_context.ServiceManager.createInstanceWithContext(
        "com.sun.star.bridge.UnoUrlResolver", local_context
    )
    context = resolver.resolve("uno:socket,host=localhost,port=2002;urp;StarOffice.ComponentContext")
    return context

def openDocument(path: Path):
    context = connect2libreOffice()
    desktop = context.ServiceManager.createInstanceWithContext(
        "com.sun.star.frame.Desktop", context
    )
    file_url = uno.systemPathToFileUrl(str(path))
    document = desktop.loadComponentFromURL(file_url, "_blank", 0, ())
    return document

# Transform 2, 4 -> C5
def getCellPosition(column, row): 
    strColumn = ""
    while column >= 0:
        strColumn = chr(column % 26 + 65) + strColumn  # 65 = ASCII 'A'
        column = column // 26 - 1 # For columns like AA

    strRow = row + 1

    return f"{strColumn}{strRow}"

def addHeaderRow(sheet):
    headerName = sheet.getCellByPosition(COLUMN_NAME, 0)
    sheet.Columns.getByIndex(0).OptimalWidth = True
    headerName.String = "<Nachname Vorname>"
    headerName.CharWeight = uno.getConstantByName("com.sun.star.awt.FontWeight.BOLD")

    headerSeasonWorkTime = sheet.getCellByPosition(COLUMN_TIME_SEASON, 0)
    sheet.Columns.getByIndex(1).OptimalWidth = True
    headerSeasonWorkTime.String = "Zeit Saison"
    headerSeasonWorkTime.CharWeight = uno.getConstantByName("com.sun.star.awt.FontWeight.BOLD")

    headerDayWorkTime = sheet.getCellByPosition(COLUMN_TIME_DAY, 0)
    sheet.Columns.getByIndex(2).OptimalWidth = True
    headerDayWorkTime.String = "Zeit Tag"
    headerDayWorkTime.CharWeight = uno.getConstantByName("com.sun.star.awt.FontWeight.BOLD")

    for i in range(0, MAX_CHECK_TIMESS):
        headerCheckIn = sheet.getCellByPosition(COLUMN_FIRST_CHECK_IN + i * 2, 0)
        sheet.Columns.getByIndex(COLUMN_FIRST_CHECK_IN + i * 2).OptimalWidth = True
        headerCheckIn.String = "Einstempeln" + str(i+1) + " [h]"
        headerCheckIn.CharWeight = uno.getConstantByName("com.sun.star.awt.FontWeight.BOLD")

        headerCheckOut = sheet.getCellByPosition(COLUMN_FIRST_CHECK_OUT + i * 2, 0)
        sheet.Columns.getByIndex(COLUMN_FIRST_CHECK_OUT + i * 2).OptimalWidth = True
        headerCheckOut.String = "Ausstemplen" + str(i+1) + " [h]"
        headerCheckOut.CharWeight = uno.getConstantByName("com.sun.star.awt.FontWeight.BOLD")

def addFormula(sheet):

    for i in range(0, MAX_NAME_ROWS):      
        rowNumber = i + 1 # Because of header line

        # Add time season 
        seasonFormula = "=("       
        seasonFormula = seasonFormula + getCellPosition(COLUMN_TIME_DAY, rowNumber)
        seasonFormula += ")*24"

        cellTimeSeason = sheet.getCellByPosition(COLUMN_TIME_SEASON, rowNumber)
        cellTimeSeason.Formula = seasonFormula

        #Add time day
        dayFormula = "="
        for j in range(0, MAX_CHECK_TIMESS):
            nextCheckInCell = getCellPosition(COLUMN_FIRST_CHECK_IN + j * 2, rowNumber)
            nextCheckOutCell = getCellPosition(COLUMN_FIRST_CHECK_OUT + j * 2, rowNumber)
            dayFormula += "IF(ISBLANK(" + nextCheckInCell + ")+ISBLANK(" + nextCheckOutCell + "); 0; "
            dayFormula += nextCheckOutCell + "-" + nextCheckInCell + ")"
            dayFormula += "+"

        # Remove last '+'
        dayFormula = dayFormula[:-1]
        cellTimeDay = sheet.getCellByPosition(COLUMN_TIME_DAY, rowNumber)
        cellTimeDay.Formula = dayFormula

def initDocument(document):
    sheets = document.Sheets
    sheet = sheets.getByIndex(0) 
    currentDate = datetime.now()
    dateFormat = "%Y-%m-%d"
    sheet.setName(currentDate.strftime(dateFormat))

    addHeaderRow(sheet)

    addFormula(sheet)
    
    for i in range(0, MAX_COLUMNS):
        sheet.Columns.getByIndex(i).OptimalWidth = True

def main(): 
    argc = len(sys.argv)
    if argc != 2:
        print("Wrong parameter count!")
        return 

    path = Path(sys.argv[1])

    if os.path.exists(path):
        print("Path already exists!")
        return

    # Creatin new .ods file failed -> Copy template instead
    # I think this issue occurs because you have to start the libreOffice-Server with other args
    script_directory = os.path.dirname(os.path.abspath(__file__))
    template_path = os.path.join(script_directory, 'template.ods') 
    shutil.copy(template_path, path)


    document = openDocument(path)

    initDocument(document)
    
    try:
        document.store() # If document already exist you can use store() instead of storeAsURL(url, properties)
    except ErrorCodeIOException:
        print("Unable to save file!")
        document.close(True)
        sys.exit(FAILED_TO_SAVE_FILE)

    document.close(True)

main()