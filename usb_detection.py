import psutil
import wmi
import time
import tkinter as tk

# Function to list all USB devices
def list_usb_devices():
    wmi_instance = wmi.WMI()  # Use lowercase 'wmi' for the instance
    usb_devices = []
    for device in wmi_instance.query("SELECT * FROM Win32_DiskDrive WHERE MediaType = 'Removable Media'"):
        usb_devices.append(device.DeviceID)  # Collect the device IDs of removable devices
        print(f"Device: {device.DeviceID}, Model: {device.Model}")
    return usb_devices

# Function to show lock screen
def show_lock_screen(lock_window):
    lock_window.deiconify()  # Show the lock screen window
    lock_window.attributes('-topmost', True)  # Keep it on top
    lock_window.attributes('-fullscreen', True)  # Fullscreen mode
    lock_window.configure(bg='black')  # Black background
    label = tk.Label(lock_window, text="System Locked\nPlease insert USB to unlock", fg='white', font=("Arial", 30))
    label.pack(expand=True)

# Function to hide the lock screen
def hide_lock_screen(lock_window):
    lock_window.withdraw()  # Hide the lock screen window

# Monitor USB devices and lock system with lock screen if removed
def monitor_usb(lock_window):
    usb_devices = list_usb_devices()

    # If no USB is detected initially, show the lock screen
    if not usb_devices:
        print("No USB detected at startup! Showing lock screen...")
        show_lock_screen(lock_window)

    def check_usb_devices():
        nonlocal usb_devices
        current_devices = list_usb_devices()

        if set(current_devices) != set(usb_devices):
            # If USB device is removed
            if usb_devices:  # If we had devices previously, that means USB was removed
                print("USB device removed! Showing lock screen...")
                show_lock_screen(lock_window)
            usb_devices = current_devices  # Update the list to reflect the USB has been removed

        elif set(current_devices) == set(usb_devices) and lock_window.winfo_ismapped():
            # If USB is detected (reinserted), hide the lock screen
            if not usb_devices:  # If no devices are detected initially
                print("USB detected. Hiding lock screen...")
                hide_lock_screen(lock_window)

        lock_window.after(1000, check_usb_devices)  # Call check_usb_devices again after 1 second

    # Start checking USB devices
    check_usb_devices()

# Setup the Tkinter window and start the monitoring process
def setup_and_monitor():
    lock_window = tk.Tk()  # Create a lock window (hidden initially)
    lock_window.withdraw()  # Hide it until needed

    # Start monitoring USB devices
    monitor_usb(lock_window)

    # Start Tkinter event loop
    lock_window.mainloop()

if __name__ == "__main__":
    setup_and_monitor()
