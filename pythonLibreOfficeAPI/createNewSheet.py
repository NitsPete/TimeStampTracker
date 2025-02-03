import sys
import uno
from pathlib import Path
from datetime import datetime
from com.sun.star.task import ErrorCodeIOException

MAX_COLUMNS = 63
MAX_ROWS = 101

COLUMN_NAME = 0
COLUMN_TIME_SEASON = 1
COLUMN_TIME_DAY = 2
COLUMN_FIRST_CHECK_IN = 3

ROW_HEADER = 0

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

def copySheetData(document):
    sheets = document.Sheets
    

    currentDate = datetime.now()
    dateFormat = "%Y-%m-%d"
    newName = currentDate.strftime(dateFormat)

    if not sheets.hasByName(newName):
        sheets.insertNewByName(newName, 0)
    else:
        print("Sheet already exist!")
        document.close(True)
        sys.exit(0)

    newSheet = sheets.getByIndex(0)  # Sheet 0 is always the newest sheet
    oldSheet = sheets.getByIndex(1)

    for columnNumber in range(0, MAX_COLUMNS):
        for rowNumber in range(0, MAX_ROWS):

            # Don't copy work times but still copy header row
            if rowNumber > ROW_HEADER and columnNumber > COLUMN_TIME_DAY:
                break

            oldCell = oldSheet.getCellByPosition(columnNumber, rowNumber)
            newCell = newSheet.getCellByPosition(columnNumber, rowNumber)

            if rowNumber == ROW_HEADER:
                newCell.String = oldCell.String
                newCell.CharWeight = oldCell.CharWeight
                newSheet.Columns.getByIndex(columnNumber).OptimalWidth = True
                continue

            if columnNumber == COLUMN_NAME:
                newCell.String = oldCell.String
                # Copy infos for special employee
                if oldCell.CellBackColor != -1:
                    newCell.CellBackColor = oldCell.CellBackColor
                # Create background cell to add time in libreOffice only if name exist
                elif newCell.String:
                    firstCheckInTimeCell = newSheet.getCellByPosition(COLUMN_FIRST_CHECK_IN, rowNumber)
                    firstCheckInTimeCell.CellBackColor = 0xFFFF00 # Yellow (RGB: 255, 255, 0)

            if columnNumber == COLUMN_TIME_SEASON:
                formula = oldCell.Formula

                # Clear old sheet day time infos
                if "+" in formula:
                    formula = formula.split('+', 1)[0]

                formula = formula + "+$'" + oldSheet.Name + "'." + getCellPosition(columnNumber, rowNumber)
                newCell.Formula = formula 
            
            if columnNumber == COLUMN_TIME_DAY:
                newCell.Formula = oldCell.Formula

        # If all names are add set width of column again 
        if columnNumber == (MAX_COLUMNS-1):
            newSheet.Columns.getByIndex(COLUMN_NAME).OptimalWidth = True


def main(): 
    argc = len(sys.argv)
    if argc != 2:
        print("Wrong parameter count!")
        return 

    path = Path(sys.argv[1])

    document = openDocument(path)

    copySheetData(document)

    try:
        document.store() # If document already exist you can use store() instead of storeAsURL(url, properties)
    except ErrorCodeIOException:
        print("Unable to save file!")
        document.close(True)
        return -1

    document.close(True)

main()