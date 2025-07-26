var sheet_id        = "1iLQ8RaCvGGr1qlOnRk-VfaYUaEy_lyw7Nme9Fz-AS70";
var allDataS        = "All_Data";
var reportS         = "Report";
var unregS          = "Unregistered";
var timeZone        = "Asia/Jakarta";
var dateTimeFormat  = "dd/MM/yyyy HH:mm:ss";

function doPost(e) {
  return handleRequest(e);
}

function doGet(e) {
  return handleRequest(e);
}

function handleRequest(e) {
  Logger.log("Received parameters: " + JSON.stringify(e.parameter));

  var ss            = SpreadsheetApp.openById(sheet_id);
  var allDataSheet  = ss.getSheetByName(allDataS);
  var reportSheet   = ss.getSheetByName(reportS);
  var unregSheet    = ss.getSheetByName(unregS);
  var Timestamp     = Utilities.formatDate(new Date(), timeZone, dateTimeFormat);

  try {
    var ID = (e.parameter.ID || e.parameter.id || "").toString().trim();
    //var Status = (e.parameter.Stat || e.parameter.stat || "").toString().trim();

    // if (!ID || !Status) {
    //   return ContentService.createTextOutput(
    //     "Error: Parameter missing → ID: [" + ID + "] Status: [" + Status + "]"
    //   ).setMimeType(ContentService.MimeType.TEXT);
    // }

    if (!ID) {
      return ContentService.createTextOutput(
        "Error: Parameter missing → ID: [" + ID + "]"
      ).setMimeType(ContentService.MimeType.TEXT);
    }

    var Nama = "Unknown";
    var Status = "Masuk";
    var isRegistered = false;

    var allData = allDataSheet.getDataRange().getValues();

    // Search ID in All_Data sheet
    for (var i = 1; i < allData.length; i++) {
      var rowID = allData[i][1]; // kolom ke-2 = ID
      if (typeof rowID !== "string") {
        rowID = rowID.toString();
      }
      rowID = rowID.trim();

      if (rowID === ID) {
        Nama = allData[i][2] || "Unknown"; // kolom ke-3 = Nama
        isRegistered = true;
        break;
      }
    }

    var detail = isRegistered ? "Registered" : "Unregistered";
    var targetSheet = isRegistered ? reportSheet : unregSheet;

    addLog(targetSheet, ID, Nama, Status, detail);

    return ContentService.createTextOutput("Success: Data was successfully saved to " + (isRegistered ? "Report" : "Unregistered"))
      .setMimeType(ContentService.MimeType.TEXT);

  } catch (error) {
    return ContentService.createTextOutput("Error: " + error.message)
      .setMimeType(ContentService.MimeType.TEXT);
  }
}

function addLog(sheet, ID, Nama, Status, Detail) {
  var Timestamp = Utilities.formatDate(new Date(), timeZone, dateTimeFormat);
  var pos = sheet.getLastRow() + 1;

  var rowData = [Timestamp, ID, Nama, Status, Detail];
  var range = sheet.getRange(pos, 1, 1, rowData.length);
  range.setValues([rowData]);
}





