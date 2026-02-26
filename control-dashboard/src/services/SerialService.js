class SerialService {
    constructor() {
        this.port = null;
        this.writer = null;
        this.reader = null;
        this.encoder = new TextEncoder();
        this.decoder = new TextDecoder();
        this.isConnected = false;

        this.commandQueue = [];
        this.isReadyToSend = false;
    }

    async connect() {
        if (!navigator.serial) {
            throw new Error("Web Serial API not supported in this browser.");
        }

        try {
            this.port = await navigator.serial.requestPort();
            await this.port.open({ baudRate: 115200 });

            this.writer = this.port.writable.getWriter();
            this.isConnected = true;

            this.readLoop();
            return true;
        } catch (error) {
            console.error("Failed to connect:", error);
            this.isConnected = false;
            throw error;
        }
    }

    async disconnect() {
        if (this.writer) {
            await this.writer.releaseLock();
        }
        if (this.port) {
            await this.port.close();
        }
        this.isConnected = false;
    }

    /**
     * Queues a configuration command.
     * The actual sending is handled by processQueue when tokens/acks allow.
     */
    async sendConfig(mode, speed, dir, brightness, var1, var2, var3, r = 0, g = 0, b = 0) {
        if (!this.isConnected || !this.writer) {
            console.error("Cannot send: Not connected");
            return;
        }

        const command = `SET:${mode},${speed},${dir},${brightness},${var1},${var2},${var3},${r},${g},${b}`;
        console.log(`Queueing command: ${command}`);
        this.commandQueue.push(command);
        if (this.commandQueue.length > 2) this.commandQueue.shift();
    }

    // Blocking Handshake Send with Retry Stream
    async sendCommand(command) {
        if (!this.isConnected || !this.writer) {
            console.warn("Serial not connected");
            return;
        }

        try {
            console.log("Requesting Pause (Stream)...");

            // 1. Create the Promise first
            const ackPromise = new Promise((resolve, reject) => {
                this.pausedResolver = resolve;
                setTimeout(() => {
                    if (this.pausedResolver) {
                        this.pausedResolver = null;
                        reject(new Error("Timeout waiting for ACK:PAUSED"));
                    }
                }, 1000); // 1s timeout
            });

            // 2. Start Streaming '?' every 10ms
            const streamInterval = setInterval(async () => {
                if (this.pausedResolver) { // Only send if still waiting
                    try {
                        await this.writer.write(this.encoder.encode("?"));
                    } catch (e) { console.error("Write error", e); }
                }
            }, 10);

            // 3. Wait for ACK
            try {
                await ackPromise;
                console.log("Pause Confirmed! Sending Data...");
            } catch (e) {
                console.warn("Handshake failed, sending anyway hoping for luck:", e);
            } finally {
                clearInterval(streamInterval); // STOP sending '?'
            }

            // Flush: Give the OS and Arduino's 64-byte HardwareSerial RX buffer
            // time to drain any residual '?' characters from the stream before 
            // blasting the 42-byte command packet that would otherwise overflow it.
            await new Promise(resolve => setTimeout(resolve, 50));

            // 4. Send Full Command safely
            const fullPacket = '<' + command + '>';
            await this.writer.write(this.encoder.encode(fullPacket));

            this.pausedResolver = null;

        } catch (error) {
            console.error("Error sending command:", error);
        }
    }

    async readLoop() {
        let buffer = "";
        while (this.port?.readable && this.isConnected) {
            this.reader = this.port.readable.getReader();
            try {
                while (true) {
                    const { value, done } = await this.reader.read();
                    if (done) break;
                    if (value) {
                        const chunk = this.decoder.decode(value); // Don't decode stream yet, just text
                        buffer += chunk;

                        // Process Buffer by lines
                        // We need to handle incomplete lines
                        const lines = buffer.split(/[\r\n]+/);

                        // If the last character wasn't a newline, the last line is incomplete.
                        // Keep it in the buffer.
                        if (!chunk.endsWith('\n') && !chunk.endsWith('\r')) {
                            buffer = lines.pop() || "";
                        } else {
                            buffer = "";
                        }

                        for (const text of lines) {
                            if (!text.trim()) continue; // Skip empty lines

                            console.log("RX:", text);

                            // Handshake Trigger (Loose match to catch partials if split weirdly, but line mode helps)
                            // "ACK:PAUSED"
                            if (text.includes("ACK:PAUSED") || text.includes("PAUSED")) {
                                if (this.pausedResolver) {
                                    this.pausedResolver();
                                    this.pausedResolver = null;
                                }
                            }

                            // Check for Token
                            if (text.includes("<RDY>")) {
                                this.processQueue();
                            }

                            if (text.includes("ACK")) console.log("CONFIRMED:", text);
                        }
                    }
                }
            } catch (error) {
                console.error("Read error:", error);
            } finally {
                this.reader.releaseLock();
            }
        }
    }

    async processQueue() {
        if (this.commandQueue.length === 0) return;
        const cmd = this.commandQueue.shift();
        console.log("Processing Queue item:", cmd);
        await this.sendCommand(cmd);
    }
}

export const serialService = new SerialService();
