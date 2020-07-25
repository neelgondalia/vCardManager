$(document).ready(function () {
    refreshLogs();
    createStatusLogMessage("Welcome! This is where the status of your commands will be displayed.", 1);

    $('#statusPanelClearBtn').click(function (e) {
        e.preventDefault();
        $('#statusLog').empty();
    });

    $('#closemodal').click(function (e) {
        e.preventDefault();
        $('#fileLabel').empty();
    });

    $('#uploadFileChooser').change(function (e) {
        e.preventDefault();
        $('#fileLabel').empty();
        $('#fileLabel').append($('#uploadFileChooser').val().replace(/^.*\\/, ""));
    });

    $('#vCardUploadForm').submit(function (e) {
        e.preventDefault();
        $('#fileLabel').empty();
        initiateUploadFileRoutine();
    });

    $('#fileSelectDropdown').change(function (e) {
        e.preventDefault();
        let filename = $('#fileSelectDropdown').val();
        refreshCardView();
        createStatusLogMessage("You are now viewing the card created from file, " + filename + ".", 1);
    });


    /************************  MY FUNCTIONS *************************/

    function initiateUploadFileRoutine() {
        let fileData = new FormData();
        let file = $('#uploadFileChooser')[0].files[0];
        let filename = $('#uploadFileChooser').val().replace(/^.*\\/, "");
        fileData.append('uploadFile', file);

        $.ajax({
            type: 'post',
            url: '/upload',
            data: fileData,
            processData: false,
            contentType: false,
            success: function (data) {
                $('#uploadModal').modal("hide");
                createStatusLogMessage("File, " + filename + ", was uploaded successfully.", 1);
                validateUploadedFile(filename);
                refreshLogs();
            },
            fail: function (error) {
                console.log(error);
                $('#uploadModal').modal("hide");
                createStatusLogMessage("Failed to upload File, " + filename + ".", 0);
            }
        });
    }

    function validateUploadedFile(uploadedfilename) {
        $.ajax({
            type: "get",
            url: "/fileLogs",
            dataType: "json",
            success: function (fileLogData) {
                $('#fileSelectDropdown').empty();

                for (let i = 0; i < fileLogData.length; i++) {
                    if (fileLogData[i].error && fileLogData[i].filename) {
                        let truncatedFileName = fileLogData[i].filename.substring(10, fileLogData[i].filename.length);
                        if (truncatedFileName == uploadedfilename) {
                            createStatusLogMessage("A Card could not be created with the uploaded file, " + uploadedfilename + ". " + fileLogData[i].error, 0);
                        }
                        continue;
                    }
                }
            },
            fail: function (error) {
                console.log("Failed To Load File!");
            }
        });
    }

    function refreshDropdown() {
        $.ajax({
            type: "get",
            url: "/fileLogs",
            dataType: "json",
            success: function (fileLogData) {
                $('#fileSelectDropdown').empty();

                for (let i = 0; i < fileLogData.length; i++) {
                    if (fileLogData[i].error) {
                        continue;
                    }
                    let truncatedFileName = fileLogData[i].filename.substring(10, fileLogData[i].filename.length);
                    let toAppendOptions = "<option class=\"dropdown-item\">" + truncatedFileName + "</option>";
                    $('#fileSelectDropdown').append(toAppendOptions);
                }
                refreshCardView();
            },
            fail: function (error) {
                console.log("Failed To Load File Logs!");
            }
        });
    }

    function refreshCardView() {
        $.ajax({
            type: "get",
            url: "/getcard",
            dataType: "json",
            data: {
                file: $('#fileSelectDropdown').val()
            },
            success: function (data) {
                $("#cardViewTable").empty();

                if ($.isEmptyObject(data) == true || data.length == 0) {
                    let toAppend = "<tr><td>No Properties Found.</td><td></td><td></td><td></td></tr>"
                    $("#cardViewTable").append(toAppend);
                    return;
                }
                for (let i = 0; i < data.length; i++) {
                    let toAppend = "<tr>" +
                        "<td>" + (i + 1) + "</td>" +
                        "<td>" + data[i].group + "</td>" +
                        "<td>" + data[i].name + "</td>" +
                        "<td>" + data[i].values + "</td>" +
                        "</tr>"

                    $("#cardViewTable").append(toAppend);
                }
            },
            fail: function (error) {
                console.log("Card failed to load.");
            }
        });
    }

    function createStatusLogMessage(message, status) {
        if (status == 0) {
            $('#statusLog').append("<div class=\"alert alert-danger alert-dismissible fade show\" role=\"alert\">" + message + "<input type=\"button\" class=\"close btn btn-danger float-right\" data-dismiss=\"alert\" aria-label=\"Close\" value=\"&times;\"></div>");
        } else if (status == 1) {
            $('#statusLog').append("<div class=\"alert alert-primary alert-dismissible fade show\" role=\"alert\">" + message + "<input type=\"button\" class=\"close btn btn-primary float-right\" data-dismiss=\"alert\" aria-label=\"Close\" value=\"&times;\"></div>");
        }

        $('#statusLog').scrollTop($('#statusLog')[0].scrollHeight);
    };

    function refreshLogs() {
        $.ajax({
            type: "get",
            url: "/fileLogs",
            dataType: "json",
            success: function (fileLogData) {
                $("#fileLogTable").empty();

                if ($.isEmptyObject(fileLogData) == true || fileLogData.length == 0) {
                    let toAppend = "<tr><td>No Files Found.</td><td></td><td></td></tr>"
                    $("#fileLogTable").append(toAppend);
                    return;
                }

                for (let i = 0; i < fileLogData.length; i++) {
                    if (fileLogData[i].error) {
                        continue;
                    }
                    let truncatedFileName = fileLogData[i].filename.substring(10, fileLogData[i].filename.length);
                    let toAppend = "<tr>" +
                        "<td><a href=\"" + fileLogData[i].filename + "\">" + truncatedFileName + "</a></td>" +
                        "<td>" + fileLogData[i].name + "</td>" +
                        "<td>" + fileLogData[i].additionalProperties + "</td>" +
                        "</tr>"

                    $("#fileLogTable").append(toAppend);
                }
                refreshDropdown();
            },
            fail: function (error) {
                console.log("Failed To Load File Logs!");
            }
        });
    }
});