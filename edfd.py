import serial
import serial.tools.list_ports
import os
import tkinter as tk
from threading import Thread, Lock
import time
import random
import pygame
from flask import Flask, render_template_string, request

# --- 1. CẤU HÌNH ---
SECRET_CODE = "1234" 
status_window = None  
boom_window = None    
is_running = False    
ser = None 
serial_lock = Lock()

try:
    pygame.mixer.init()
    def play_alarm():
        if os.path.exists("naval-alarm.mp3"):
            pygame.mixer.music.load("naval-alarm.mp3"); pygame.mixer.music.play(-1)
    def stop_alarm():
        if pygame.mixer.music.get_busy(): pygame.mixer.music.stop()
except: pass

FAKE_LOGS = [">> WIPING DATA", ">> THERMAL MAX", ">> CORE_OVERLOAD", ">> ENCRYPTION: SHREDDED", ">> REAPING CORES", ">> DISK_WIPE: 99%"]

# --- 2. GIAO DIỆN WEB ---
HTML_UI = """
<!DOCTYPE html>
<html>
<head>
    <title>BOOM</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { background: #050505; color: #00ff00; font-family: 'Consolas', monospace; text-align: center; padding: 20px; }
        .box { border: 2px solid #222; padding: 20px; background: #0a0a0a; max-width: 400px; margin: auto; }
        h1 { color: #ff0000; font-family: 'Impact'; font-size: 28px; }
        .btn { display: block; width: 100%; margin: 15px 0; padding: 20px; font-weight: bold; cursor: pointer; border: 1px solid #333; text-transform: uppercase; }
        .btn-act { background: #f1c40f; color: #000; font-family: 'Impact'; }
        .btn-boom { background: #600; color: #fff; border: 2px solid #f00; font-family: 'Impact'; }
        .btn-off { background: #111; color: #0f0; border-color: #0f0; }
    </style>
</head>
<body>
    <div class="box">
        <h1>☢ BOOM ☢</h1>
        <button class="btn btn-act" onclick="fetch('/command?type=activate')">PREPARE SYSTEM</button>
        <button class="btn btn-boom" onclick="fetch('/command?type=destruct&code=1234')">KÍCH HOẠT TỰ HỦY</button>
        <button class="btn btn-off" onclick="fetch('/command?type=clear')">ABORT MISSION</button>
    </div>
</body>
</html>
"""

# --- 3. HÀM XỬ LÝ GIAO DIỆN ---
def send_to_esp(cmd):
    global ser
    with serial_lock:
        if ser and ser.is_open:
            try: ser.write(f"{cmd}\n".encode()); ser.flush()
            except: pass

def clear_all_overlays():
    global is_running, status_window, boom_window
    if status_window:
        try: status_window.destroy()
        except: pass
        status_window = None
    if boom_window:
        try: boom_window.destroy()
        except: pass
        boom_window = None
    is_running = False
    stop_alarm()
    send_to_esp("L_OFF")

def create_status_overlay():
    global status_window
    if is_running or (status_window and status_window.winfo_exists()): return
    send_to_esp("L_YELLOW")
    status_window = tk.Toplevel()
    status_window.overrideredirect(True); status_window.attributes("-topmost", True)
    sw = status_window.winfo_screenwidth()
    status_window.geometry(f"450x100+{sw-470}+20")
    lbl = tk.Label(status_window, text="⚠️ SYSTEM ARMED\nREADY FOR PURGE", fg="black", bg="#f1c40f", font=("Impact", 18))
    lbl.pack(expand=True, fill="both")
    def blink():
        if status_window and status_window.winfo_exists() and not is_running:
            cur = lbl.cget("bg"); nxt = "black" if cur == "#f1c40f" else "#f1c40f"
            lbl.config(bg=nxt, fg="#f1c40f" if cur == "#f1c40f" else "black")
            status_window.after(500, blink)
    blink()

def start_self_destruct():
    global boom_window, is_running
    if is_running: return
    is_running = True
    if status_window: 
        try: status_window.destroy()
        except: pass
    
    send_to_esp("L_RED")
    play_alarm()
    
    root = tk.Toplevel(); boom_window = root
    root.attributes("-topmost", True, "-fullscreen", True); root.configure(bg='#7b0000'); root.config(cursor="none")
    
    # --- KHÔI PHỤC ĐẦY ĐỦ CHI TIẾT GIAO DIỆN ---
    # 1. Dải Warning đen phía trên
    tk.Label(root, text="[!] WARNING: SYSTEM DESTRUCTION IN PROGRESS [!] ", fg="white", bg="black", font=("Impact", 20)).place(relx=0.5, rely=0.05, anchor="center", relwidth=1.0)
    
    # 2. Thông tin Tọa độ & ID
    tk.Label(root, text="LAT: 10.7626° N\nLON: 106.6602° E\nID: 77-REAPER\nAUTH: GRANTED", fg="white", bg="#7b0000", font=("Consolas", 12), justify="left").place(x=20, y=120)
    
    # 3. Tiêu đề chính
    tk.Label(root, text="☢ NUCLEAR PURGE ☢", fg="white", bg="#7b0000", font=("Impact", 45)).place(relx=0.5, rely=0.35, anchor="center")
    
    # 4. Phần trăm tiến độ
    p_lbl = tk.Label(root, text="0.0%", fg="black", bg='#7b0000', font=("Impact", 180))
    p_lbl.place(relx=0.5, rely=0.55, anchor="center")
    
    # 5. Thanh Progress Bar đỏ rực
    p_bg = tk.Frame(root, bg="black", height=45); p_bg.place(relx=0.5, rely=0.75, anchor="center", relwidth=0.7)
    p_bar = tk.Frame(p_bg, bg="#ff0000", height=45); p_bar.place(x=0, y=0, width=0)

    # 6. Khu vực Log nhật ký bên phải
    log_c = tk.Frame(root, bg="#7b0000"); log_c.place(relx=0.95, rely=0.2, anchor="ne")

    def update_p(curr_t):
        if not is_running or not boom_window: return
        if curr_t > 0:
            p = min(100.0, (10.0 - curr_t) * 10)
            p_lbl.config(text=f"{p:.1f}%")
            bar_w = int((root.winfo_screenwidth() * 0.7) * (p / 100))
            p_bar.place(width=bar_w)
            root.after(10, lambda: update_p(curr_t - 0.01))
        else: os.system("shutdown /s /t 0")

    def add_log():
        if not is_running or not boom_window: return
        tk.Label(log_c, text=random.choice(FAKE_LOGS), fg="white", bg="#7b0000", font=("Consolas", 12)).pack(anchor="e")
        if len(log_c.winfo_children()) > 12: log_c.winfo_children()[0].destroy()
        root.after(300, add_log)
    
    add_log(); update_p(10.0)

# --- 4. SERIAL & WEB ---
def listen_serial():
    global ser
    while True:
        ports = serial.tools.list_ports.comports()
        p = next((p.device for p in ports if any(x in p.description.upper() for x in ["USB", "SERIAL", "CH340"])), None)
        if p:
            try:
                with serial_lock: ser = serial.Serial(p, 115200, timeout=0.1)
                print(f">> ĐÃ KẾT NỐI VỚI ESP32 TẠI {p}")
                while True:
                    if ser and ser.in_waiting > 0:
                        line = ser.readline().decode('utf-8', errors='ignore').strip()
                        # SỬA LỖI NÚT LA38: Dùng 'startswith' thay vì '==' để nhận diện lệnh có tọa độ
                        if line == "SYSTEM_ACTIVATED": main_root.after(0, create_status_overlay)
                        elif line.startswith("ACTIVATE_SELF_DESTRUCT"): main_root.after(0, start_self_destruct)
                        elif line in ["ABORT", "SYSTEM_LOCKED"]: main_root.after(0, clear_all_overlays)
                    time.sleep(0.01)
            except: ser = None
        time.sleep(1)

app = Flask(__name__)
@app.route('/')
def index(): return render_template_string(HTML_UI)

@app.route('/command')
def handle_cmd():
    t, c = request.args.get('type'), request.args.get('code', "")
    if t == "destruct" and c != SECRET_CODE: return "WRONG", 200
    if t == "activate": main_root.after(0, create_status_overlay)
    elif t == "destruct": main_root.after(0, start_self_destruct)
    elif t == "clear": main_root.after(0, clear_all_overlays)
    return "OK"

if __name__ == "__main__":
    main_root = tk.Tk(); main_root.withdraw()
    Thread(target=listen_serial, daemon=True).start()
    Thread(target=lambda: app.run(host='0.0.0.0', port=5000), daemon=True).start()
    main_root.mainloop()