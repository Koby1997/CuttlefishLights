const { app, BrowserWindow, ipcMain } = require('electron');
const path = require('path');
const { autoUpdater } = require('electron-updater');

let mainWindow;

function createWindow() {
  const isDev = !app.isPackaged;

  mainWindow = new BrowserWindow({
    width: 1200,
    height: 800,
    icon: path.join(__dirname, isDev ? '../public/logo.png' : '../dist/logo.png'),
    frame: false,
    webPreferences: {
      preload: path.join(__dirname, 'preload.cjs'),
      contextIsolation: true,
      nodeIntegration: false,
    },
  });

  if (isDev) {
    mainWindow.loadURL('http://localhost:5173');
    // Open the DevTools by default in dev mode
    // mainWindow.webContents.openDevTools();
  } else {
    mainWindow.loadFile(path.join(__dirname, '../dist/index.html'));
  }

  // Add keyboard shortcut for DevTools (Ctrl+Shift+I)
  mainWindow.webContents.on('before-input-event', (event, input) => {
    if (input.control && input.shift && input.key.toLowerCase() === 'i') {
      mainWindow.webContents.toggleDevTools();
      event.preventDefault();
    }
  });

  // Web Serial API permission handler
  mainWindow.webContents.session.on('select-serial-port', (event, portList, webContents, callback) => {
    // Implement cancellation handler
    mainWindow.webContents.session.on('serial-port-cancelled', () => {
      callback('');
    });

    console.log('Available serial ports:', portList.map(p => ({ portId: p.portId, vendorId: p.vendorId })));

    // Common Arduino/USB-Serial vendor IDs
    const arduinoVendorIds = ['2341', '2a03', '1a86', '10c4', '0403'];

    const arduinoPorts = portList.filter(port => 
      port.vendorId && arduinoVendorIds.includes(port.vendorId.toLowerCase())
    );

    if (arduinoPorts.length > 0) {
      console.log('Auto-connecting to detected Arduino:', arduinoPorts[0].portId);
      event.preventDefault(); 
      callback(arduinoPorts[0].portId);
    } else if (portList.length > 0) {
      console.log('No known Arduino detected, requesting React UI selection...');
      event.preventDefault();
      
      global.serialPortCallback = callback;
      mainWindow.webContents.send('serial-ports-available', portList);
    } else {
      callback(''); // No ports found
    }
  });

  mainWindow.webContents.session.setPermissionCheckHandler((webContents, permission, requestingOrigin, details) => {
    if (permission === 'serial') return true;
    return true;
  });

  mainWindow.webContents.session.setDevicePermissionHandler((details) => {
    if (details.deviceType === 'serial') return true;
    return true;
  });
}

ipcMain.on('serial-port-selected', (event, portId) => {
  if (global.serialPortCallback) {
    global.serialPortCallback(portId);
    global.serialPortCallback = null;
  }
});

ipcMain.on('serial-port-cancelled', (event) => {
  if (global.serialPortCallback) {
    global.serialPortCallback('');
    global.serialPortCallback = null;
  }
});

// Window Controls
ipcMain.on('window-minimize', (event) => BrowserWindow.fromWebContents(event.sender)?.minimize());
ipcMain.on('window-maximize', (event) => {
  const win = BrowserWindow.fromWebContents(event.sender);
  if (!win) return;
  if (win.isMaximized()) win.restore(); else win.maximize();
});
ipcMain.on('window-close', (event) => BrowserWindow.fromWebContents(event.sender)?.close());

// --- Auto-Updater ---
function sendStatusToWindow(text) {
  mainWindow?.webContents.send('update-status', text);
}

autoUpdater.on('checking-for-update', () => {
  sendStatusToWindow('Checking for updates...');
});
autoUpdater.on('update-available', (info) => {
  sendStatusToWindow(`New version found! Downloading...`);
});
autoUpdater.on('update-not-available', (info) => {
  sendStatusToWindow('Currently running the latest version.');
});
autoUpdater.on('error', (err) => {
  mainWindow?.webContents.executeJavaScript(`console.error("AutoUpdater backend error:", \`${err.stack || err.message}\`)`);
  sendStatusToWindow('Error while checking.');
});
autoUpdater.on('download-progress', (progressObj) => {
  sendStatusToWindow(`Downloading... ${Math.round(progressObj.percent)}%`);
});
autoUpdater.on('update-downloaded', (info) => {
  sendStatusToWindow('Restarting to update...');
  setTimeout(() => {
    autoUpdater.quitAndInstall();
  }, 2000);
});

ipcMain.on('check-for-updates', () => {
  if (app.isPackaged) {
    autoUpdater.checkForUpdatesAndNotify();
  } else {
    sendStatusToWindow('Updates disabled in Dev mode.');
  }
});

app.whenReady().then(() => {
  createWindow();

  app.on('activate', function () {
    if (BrowserWindow.getAllWindows().length === 0) createWindow();
  });
});

app.on('window-all-closed', function () {
  if (process.platform !== 'darwin') app.quit();
});
