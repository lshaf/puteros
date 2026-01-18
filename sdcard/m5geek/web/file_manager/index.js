function $(s) { return document.querySelector(s) }
const IS_DEV = (window.location.host === "127.0.0.1:8080");
const T = {
  master: $('#t'),
  fileRow: function () {
    const tmp = document.createElement('template');
    tmp.innerHTML = this.master.content.querySelector('table tr.file-row').outerHTML;
    return tmp.content;
  },
  uploadLoading: function () {
    const tmp = document.createElement('template');
    tmp.innerHTML = this.master.content.querySelector('.upload-loading').outerHTML;
    return tmp.content;
  }
};

const Dialog = {
  _bg: function (show) {
    let bg = $(".dialog-background");
    let dialogs = document.querySelectorAll(".dialog");
    dialogs.forEach((dialog) => {
      if (!dialog.classList.contains("hidden"))
        dialog.classList.add("hidden");
    });
    if (show) {
      bg.classList.remove("hidden");
    } else {
      bg.classList.add("hidden");
    }
  },
  show: function (dialogName) {
    this._bg(true);
    let dialog = $(".dialog." + dialogName);
    dialog.classList.remove("hidden");
  },
  hide: function () {
    this._bg(false);
    this.loading.hide();
  },
  loading: {
    show: function (message) {
      $(".loading-area").classList.remove("hidden");
      $(".loading-area .text").textContent = message || "Loading...";
    },
    hide: function () {
      $(".loading-area").classList.add("hidden");
    }
  },
  showOneInput: function (name) {
    const dbForm = {
      renameFolder: {
        title: "Rename Folder",
        label: `New Name:`,
        action: "Rename"
      },
      renameFile: {
        title: "Rename File",
        label: `New Name:`,
        action: "Rename"
      },
      createFolder: {
        title: "Create Folder",
        label: `Folder Name:`,
        action: "Create Folder"
      },
      createFile: {
        title: "Create File",
        label: `File Name:`,
        action: "Create File"
      },
      serial: {
        title: "Serial Command",
        label: `Command:`,
        action: "Run"
      }
    };

    let config = dbForm[name];
    if (!config) {
      alert("Invalid dialog name: " + name);
      console.error("Dialog.showOneInput: Invalid dialog name", name);
      return;
    }

    let dialog = $(".dialog.oinput");
    dialog.querySelector(".oinput-title").textContent = config.title;
    dialog.querySelector(".oinput-label").textContent = config.label;
    dialog.querySelector(".oinput-file-name").textContent = "";
    dialog.querySelector(".act-save-oinput-file").textContent = config.action;
    this.show('oinput');
    dialog.querySelector("#oinput-input").value = "";
    dialog.querySelector("#oinput-input").focus();
    return dialog;
  }
};

async function requestPost (url, param) {
  return new Promise((resolve, reject) => {
    let fd = new FormData();
    for (let key in param) {
      fd.append(key, param[key]);
    }

    let realUrl = url;
    if (IS_DEV) realUrl = "/m5geek" + url;
    let req = new XMLHttpRequest();
    req.open("POST", realUrl, true);
    req.withCredentials = true;
    req.onload = () => {
      if (req.status >= 200 && req.status < 300) {
        resolve(req.responseText);
      } else {
        reject(new Error(`Request failed with status ${req.status}`));
      }
    };
    req.onerror = () => reject(new Error("Network error"));
    req.send(fd);
  });
}

function stringToId(str) {
  let hash = 0, i, chr;
  if (str.length === 0) return hash.toString();
  for (i = 0; i < str.length; i++) {
    chr   = str.charCodeAt(i);
    hash  = ((hash << 5) - hash) + chr;
    hash |= 0; // Convert to 32bit integer
  }
  return 'id_' + Math.abs(hash);
}

const _queueUpload = [];
let _runningUpload = false;
function appendFileToQueue(files) {
  Dialog.show('upload');
  let d = $(".dialog.upload");
  for (let i = 0; i < files.length; i++) {
    let file = files[i];
    let filename = file.webkitRelativePath || file.name;
    let fileId = stringToId(filename);
    let progressBar = T.uploadLoading();
    progressBar.querySelector(".upload-name").textContent = filename;
    progressBar.querySelector(".upload-loading .bar").setAttribute("id", fileId);

    d.querySelector(".dialog-body").appendChild(progressBar);
  }
}
async function appendDroppedFiles(entry) {
  return new Promise((resolve, reject) => {
    if (entry.isFile) {
      entry.file((file) => {
        let fileWithPath = new File([file], entry.fullPath.substring(1), { type: file.type });
        appendFileToQueue([fileWithPath]);
        _queueUpload.push(fileWithPath);
        resolve();
      });
    } else if (entry.isDirectory) {
      let proms = [];
      let reader = entry.createReader();
      reader.readEntries((entries) => {
        for (let e of entries) proms.push(appendDroppedFiles(e));
      });

      Promise.all(proms).then(resolve);
    }
  })
}
async function uploadFile () {
  if (_queueUpload.length === 0) {
    _runningUpload = false;
    $(".dialog.upload .dialog-body").innerHTML = "";
    await fetchSystemInfo();
    await fetchFiles(currentPath);
    Dialog.hide();
    return;
  }

  return new Promise((resolve, reject) => {
    _runningUpload = true;
    let file = _queueUpload.shift();
    let fd = new FormData();
    let filename = file.webkitRelativePath || file.name;
    let fileId = stringToId(filename);
    fd.append("file", file, filename);
    fd.append("folder", currentPath);

    let realUrl = `/upload`;
    if (IS_DEV) realUrl = "/m5geek" + realUrl;
    let req = new XMLHttpRequest();
    req.upload.onprogress = (e) => {
      if (e.lengthComputable) {
        var percent = (e.loaded / e.total) * 100;
        $("#" + fileId).style.width = Math.round(percent) + "%";
      }
    };
    req.onload = () => {
      uploadFile();
      if (req.status >= 200 && req.status < 300) {
        resolve(req.responseText);
      } else {
        reject();
      }
    };
    req.onabort = () => reject();
    req.onerror = () => reject();
    req.open("POST", realUrl, true);
    req.send(fd);
  });
}

function calcHash(str) {
  let hash = 5381;
  str = str.replace(/\r\n/g, '\n').replace(/\r/g, '\n');
  for (let i = 0; i < str.length; i++) {
    hash = ((hash << 5) + hash) ^ str.charCodeAt(i); // djb2 xor variant
    hash = hash >>> 0; // force unsigned 32-bit
  }

  return hash.toString(16).padStart(8, '0');
}

function renderFileRow(fileList) {
  $("table.explorer tbody").innerHTML = "";
  let sortedList = fileList.split("\n").sort((a, b) => {
    return a.localeCompare(b);
  })

  if (currentPath !== "/") sortedList = ["DIR:..:0", ...sortedList];

  sortedList
    .forEach((line) => {
    let e;
    let [type, name, size] = line.split(":");
    if (size === undefined) return;
    let dPath = ((currentPath.endsWith("/") ? currentPath : currentPath + "/") + name).replace(/\/\//g, "/");
    size = formatBytes(parseInt(size));
    if (name === "..") {
      dPath = currentPath.substring(0, currentPath.lastIndexOf("/"));
      if (dPath === "") dPath = "/";
    }

    if (type === "FILE") {
      e = T.fileRow();
      e.querySelector('.file-row').setAttribute("data-file", dPath);
      e.querySelector('.act-rename').setAttribute("data-action", "renameFile");
      e.querySelector(".col-name").classList.add("act-edit-file");
      e.querySelector(".col-name").textContent = name;
      e.querySelector(".col-name").setAttribute("title", name);
      e.querySelector(".col-size").textContent = size;
      e.querySelector(".col-action").classList.add("type-file");

      let downloadUrl = `/download?file=${encodeURIComponent(dPath)}`;
      if (IS_DEV) downloadUrl = "/m5geek" + downloadUrl;
      e.querySelector(".act-download").setAttribute("download", name);
      e.querySelector(".act-download").setAttribute("href", downloadUrl);
    } else if (type === "DIR") {
      e = T.fileRow();
      e.querySelector(".col-name").classList.add("act-browse");
      e.querySelector('.file-row').setAttribute("data-path", dPath);
      e.querySelector(".col-action").classList.add("type-folder");
      e.querySelector(".col-name").textContent = name;
      e.querySelector(".col-name").setAttribute("title", name);
      if (name !== "..") {
        e.querySelector('.act-rename').setAttribute("data-action", "renameFolder");
      } else {
        e.querySelector(".col-action").innerHTML = "";
      }
    }
    $("table.explorer tbody").appendChild(e);
  });
}

let currentPath;
async function fetchFiles(path) {
  currentPath = path;
  $(`.act-browse.active`)?.classList.remove("active");
  $(".current-path").textContent = "SDCard:/" + path;
  Dialog.loading.show('Fetching files...');
  let req = await requestPost("/", {command: "ls", path});
  renderFileRow(req);
  Dialog.loading.hide();
}

function formatBytes(bytes) {
  if (bytes === 0) return '0 B';

  const k = 1024;
  const sizes = ['B', 'KB', 'MB', 'GB'];
  const i = Math.floor(Math.log(bytes) / Math.log(k));

  return Math.round((bytes / Math.pow(k, i)) * 100) / 100 + ' ' + sizes[i];
}

async function fetchSystemInfo() {
  Dialog.loading.show('Fetching system info...');
  let req = await requestPost("/", {command: "sysinfo"});
  let data = req.split("\n");
  let usedSpace = parseInt(data[2].split(":")[1]);
  let totalSpace = parseInt(data[3].split(":")[1]);
  let appVersion = data[4].split(":")[1];
  $(".free-space .free-sd span").innerHTML = `${formatBytes(usedSpace)} / ${formatBytes(totalSpace)}`;
  $(".m5geek-version").innerHTML = appVersion;
  Dialog.loading.hide();
}

async function saveEditorFile() {
  Dialog.loading.show('Saving...');
  let editor = $(".dialog.editor .file-content");
  let filename = $(".dialog.editor .editor-file-name").textContent.trim();
  if (isModified(editor)) {
    $(".act-save-edit-file").disabled = true;
    editor.setAttribute("data-hash", calcHash(editor.value));
    await requestPost("/", {command: "echo", path: filename, content: editor.value});
  }

  Dialog.loading.hide();
}

function isModified(target) {
  let oldHash = target.getAttribute("data-hash");
  let newHash = calcHash(target.value);
  return oldHash !== newHash;
}

window.ondragenter = () => $(".upload-area").classList.remove("hidden");
$(".upload-area").ondragleave = () => $(".upload-area").classList.add("hidden");
$(".upload-area").ondragover = (e) => e.preventDefault();
$(".upload-area").ondrop = async (e) => {
  e.preventDefault();
  $(".upload-area").classList.add("hidden")
  const items = e.dataTransfer.items;
  if (!items || items.length === 0) return;

  for (let i of items) {
    let entry = i.webkitGetAsEntry();
    if (!entry) continue;
    await appendDroppedFiles(entry);
  }

  if (!_runningUpload) setTimeout(() => {
    if (_queueUpload.length === 0) return;
    uploadFile();
  }, 100);
};

document.querySelectorAll(".inp-uploader").forEach((el) => {
  el.addEventListener("change", async (e) => {
    let files = e.target.files;
    if (!files || files.length === 0) return;

    appendFileToQueue(files);
    _queueUpload.push(...files);
    if (!_runningUpload) uploadFile();

    this.value = "";
  });
});

$(".container").addEventListener("click", async (e) => {
  let browseAction = e.target.closest(".act-browse");
  if (browseAction) {
    e.preventDefault();
    let path = browseAction.getAttribute("data-path")
      || browseAction.closest("tr").getAttribute('data-path')
      || "/";
    if (path === currentPath) return;

    await fetchFiles(path);
    return;
  }

  let editFileAction = e.target.closest(".act-edit-file");
  if (editFileAction) {
    e.preventDefault();
    let editor = $(".dialog.editor .file-content");
    let file = editFileAction.closest("tr").getAttribute("data-file");
    if (!file) return;
    $(".dialog.editor .editor-file-name").textContent = file;
    editor.value = "";

    // Load file content
    Dialog.loading.show('Fetching content...');
    let r = await requestPost("/", {command: "cat", path: file});
    editor.value = r;
    editor.setAttribute("data-hash", calcHash(r));

    $(".act-save-edit-file").disabled = true;
    Dialog.loading.hide();
    Dialog.show('editor');
    return;
  }

  let oActionOInput = e.target.closest(".act-oinput");
  if (oActionOInput) {
    e.preventDefault();
    let action = oActionOInput.getAttribute("data-action");
    if (!action) return;

    let filePath = currentPath;
    let d = Dialog.showOneInput(action);
    if (action.startsWith("rename")) {
      let row = oActionOInput.closest("tr");
      filePath = row.getAttribute("data-file") || row.getAttribute("data-path");
    } else if (action === "serial") {
      filePath = "";
    }

    d.setAttribute("data-cache", `${action}|${filePath}`);
    if (filePath != "") {
      let fName = filePath.substring(filePath.lastIndexOf("/") + 1);
      let fNameSpan = d.querySelector(".oinput-file-name");
      fNameSpan.textContent = ": " + fName;
      fNameSpan.setAttribute("title", fName);
      $("#oinput-input").value = fName;
    }

    return;
  }

  let actDeleteFile = e.target.closest(".act-delete");
  if (actDeleteFile) {
    e.preventDefault();
    let file = actDeleteFile.closest(".file-row").getAttribute("data-file")
      || actDeleteFile.closest(".file-row").getAttribute("data-path");
    if (!file) return;

    if (!confirm(`Are you sure you want to DELETE ${file}?\n\nTHIS ACTION CANNOT BE UNDONE!`)) return;

    Dialog.loading.show('Deleting...');
    await requestPost("/", {command: "rm", path: file});
    await fetchSystemInfo();
    await fetchFiles(currentPath);
    Dialog.loading.hide();
    return;
  }
});


$(".dialog-background").addEventListener("click", async (e) => {
  if (e.target.matches(".act-dialog-close")) {
    e.preventDefault();
    Dialog.hide();
    return;
  }
});

$(".act-save-oinput-file").addEventListener("click", async (e) => {
  let dialog = $(".dialog.oinput");
  let fileInput = $("#oinput-input");
  let fileName = fileInput.value.trim();
  if (!fileName) {
    alert("Filename cannot be empty.");
    return;
  }
  let action = dialog.getAttribute("data-cache");
  if (!action) {
    alert("No action specified.");
    return;
  }

  let refreshList = true;
  let [actionType, path] = action.split("|");
  if (actionType.startsWith("rename")) {
    Dialog.loading.show('Renaming...');
    let destPath = path.substring(0, path.lastIndexOf("/") + 1) + fileName;
    await requestPost("/", {command: "mv", src: path, dst: destPath});
  } else if (actionType === "createFolder") {
    Dialog.loading.show('Creating Folder...');
    await requestPost("/", {command: "mkdir", path: path.trimEnd("/") + "/" + fileName});
  } else if (actionType === "createFile") {
    Dialog.loading.show('Creating File...');
    await requestPost("/", {command: "touch", path: path.trimEnd("/") + "/" + fileName});
  }

  if (refreshList) fetchFiles(currentPath);
  Dialog.hide();
});

$(".act-save-edit-file").addEventListener("click", async (e) => {
  await saveEditorFile();
});

$(".act-auth-login").addEventListener("click", async (e) => {
  let pass = $("#auth-password").value;
  if (!pass) {
    alert("Password cannot be empty.");
    return;
  }

  Dialog.loading.show('Authenticating...');
  try {
    await requestPost("/", {command: "sudo", param: pass});
    Dialog.hide();
    fetchSystemInfo();
    fetchFiles("/");
  } catch (error) {
    alert("Authentication error: " + error.message);
    console.error("Authentication error:", error);
  } finally {
    Dialog.loading.hide();
  }
});

$("#act-btn-logout").addEventListener("click", async (e) => {
  if (!confirm("Are you sure you want to logout?")) return;

  Dialog.loading.show('Logging out...');
  try {
    await requestPost("/", {command: "exit"});
    Dialog.show("auth");
  } catch (error) {
    alert("Logout error: " + error.message);
    console.error("Logout error:", error);
  } finally {
    Dialog.loading.hide();
  }
})

window.addEventListener("keydown", async (e) => {
  let key = e.key.toLowerCase()
  if ($(".dialog.editor:not(.hidden)")) { // means editor tab is open
    if ((e.ctrlKey || e.metaKey) && key === "s") {
      e.preventDefault();
      e.stopImmediatePropagation();

      await saveEditorFile();
    }
  }

  if (key === "escape" && $(".dialog-background:not(.hidden)")) {
    if ($(".dialog.editor:not(.hidden)")) {
      let editor = $(".dialog.editor .file-content");
      if (isModified(editor)) {
        if (!confirm("You have unsaved changes. Do you want to discard them?")) {
          return;
        }
      }
    }

    let btnEscape = $(".dialog:not(.hidden) .act-escape");
    if (btnEscape) btnEscape.click();
    return;
  }
});

$(".file-content").addEventListener("keyup", function (e) {
  if ($(".dialog.editor:not(.hidden)")) {
    // map special characters to their closing pair
    map_chars = {
      "(": ")",
      "{": "}",
      "[": "]",
      '"': '"',
      "'": "'",
      "`": "`",
      "<": ">"
    };

    // if the key pressed is a special character, insert the closing pair
    if (e.key in map_chars) {
      var cursorPos = this.selectionStart;
      var textBefore = this.value.substring(0, cursorPos);
      var textAfter = this.value.substring(cursorPos);
      this.value = textBefore + map_chars[e.key] + textAfter;
      this.selectionStart = cursorPos;
      this.selectionEnd = cursorPos;
    }

    $(".act-save-edit-file").disabled = !isModified(e.target);
  }
});

(async function () {
  try {
    await fetchSystemInfo();
    await fetchFiles("/");
  } catch (e) {
    Dialog.loading.hide();
    Dialog.show("auth");
  }
})();
