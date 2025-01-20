var sheet_id        = "1z0P3rhtJDsABbchxLFNISHEN0UoqoIHJg2b-QYJRqwA";
var enrollS         = "Enroll";
var reportS         = "Report";
var siswaS          = "Data_Siswa";
var timeZone        = "Asia/Jakarta";
var dateTimeFormat  = "dd/MM/yyyy HH:mm:ss";

function doPost(e) {
  var ss          = SpreadsheetApp.openById(sheet_id);
  var sheet       = ss.getSheetByName(enrollS);
  var siswaSheet  = ss.getSheetByName(siswaS);
  var Timestamp   = Utilities.formatDate(new Date(), timeZone, dateTimeFormat);

  try {
    var ID_Siswa = e.parameter.ID_Siswa || "Unknown";
    var Nama = '', Kelas = '';

    // Ambil data dari sheet "Data_Siswa"
    var siswaData     = siswaSheet.getDataRange().getValues();
    var idSiswaFound  = false;

    // Cari ID_Siswa di sheet "Data_Siswa"
    for (var i = 1; i < siswaData.length; i++) {
      if (siswaData[i][0].toString() === ID_Siswa.toString()) {
        Nama          = siswaData[i][1];
        Kelas         = siswaData[i][2];
        idSiswaFound  = true;
        break;
      }
    }

    if (!idSiswaFound) {
      Nama  = "Unknown";
      Kelas = "Unknown";
    }

    var Status = e.parameter.Status || "Unknown";

    addLog(sheet, ID_Siswa, Nama, Kelas, Status);

    return ContentService.createTextOutput("Success: Data berhasil disimpan ke Enroll").setMimeType(ContentService.MimeType.TEXT);
  } catch (error) {
    return ContentService.createTextOutput("Error: " + error.message).setMimeType(ContentService.MimeType.TEXT);
  }
}

function doGet(e) {
  var ss          = SpreadsheetApp.openById(sheet_id);
  var enrollSheet = ss.getSheetByName(enrollS);
  var reportSheet = ss.getSheetByName(reportS);
  var Timestamp   = Utilities.formatDate(new Date(), timeZone, dateTimeFormat);

  try {
    var ID_Siswa  = e.parameter.ID_Siswa || "Unknown";
    var Status    = e.parameter.Status || "Unknown";

    // Verifikasi ID dari sheet "Enroll"
    var enrollData  = enrollSheet.getDataRange().getValues();
    var idFound     = false;

    for (var i = 1; i < enrollData.length; i++) {
      if (enrollData[i][1].toString() === ID_Siswa.toString()) {
        idFound = true;
        Nama    = enrollData[i][2] || "Unknown";
        Kelas   = enrollData[i][3] || "Unknown";
        break;
      }
    }

    if (!idFound) {
      return ContentService.createTextOutput("ID tidak ditemukan di Enroll").setMimeType(ContentService.MimeType.TEXT);
    }

    addLog(reportSheet, ID_Siswa, Nama, Kelas, Status);

    return ContentService.createTextOutput("Success: Data berhasil disimpan ke Report").setMimeType(ContentService.MimeType.TEXT);
  } catch (error) {
    return ContentService.createTextOutput("Error: " + error.message).setMimeType(ContentService.MimeType.TEXT);
  }
}

function addLog(sheet, logIDSValue, logNamaValue, logKelasValue, logStatusValue) {
  var pos     = sheet.getLastRow() + 1;
  var rowData = [];

  rowData[0] = Utilities.formatDate(new Date(), timeZone, dateTimeFormat);
  rowData[1] = logIDSValue;
  rowData[2] = logNamaValue;
  rowData[3] = logKelasValue;
  rowData[4] = logStatusValue;

  var newRange = sheet.getRange(pos, 1, 1, rowData.length);
  newRange.setValues([rowData]);
}
