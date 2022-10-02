from PyQt5.QAxContainer import QAxObject

excel = QAxObject("Excel.Application")
books = excel.querySubObject("Workbooks")
book = books.querySubObject("Add()")
sheets = book.querySubObject("Sheets")
sheet = sheets.querySubObject("Item(int)", 1)

excel.setProperty("Visible", True)