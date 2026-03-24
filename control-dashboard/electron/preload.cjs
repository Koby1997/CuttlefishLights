const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('electronAPI', {
  onSerialPortsAvailable: (callback) => {
    // Remove previous listeners to prevent duplicates
    ipcRenderer.removeAllListeners('serial-ports-available');
    ipcRenderer.on('serial-ports-available', (_event, ports) => callback(ports));
  },
  selectSerialPort: (portId) => ipcRenderer.send('serial-port-selected', portId),
  cancelSerialPort: () => ipcRenderer.send('serial-port-cancelled'),
  windowMinimize: () => ipcRenderer.send('window-minimize'),
  windowMaximize: () => ipcRenderer.send('window-maximize'),
  windowClose: () => ipcRenderer.send('window-close'),

  // Updater commands
  checkForUpdates: () => ipcRenderer.send('check-for-updates'),
  onUpdateStatus: (callback) => {
    ipcRenderer.removeAllListeners('update-status');
    ipcRenderer.on('update-status', (_event, text) => callback(text));
  }
});
