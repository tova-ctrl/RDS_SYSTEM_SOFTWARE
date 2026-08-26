
using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;

namespace RDS_WebApp.Services
{
    // הגדרת המצבים האפשריים של המערכת
    public enum SystemState { SAFE, STANDBY, ARMED, FIRING, FAULT, COMM_LOSS }


    public class SystemControlService
    {
        public SystemState CurrentState { get; private set; } = SystemState.SAFE;

        public bool CameraOn { get; private set; } = false;

        public List<string> Logs { get; } = new();

        public event Action? OnChange;

        // 2026-08-16: B2 (FIRE) — mapping's own spec (see [[mobilicom-controller-mapping]]
        // memory) says the fire window "auto-closes after 20s with no firing". Without
        // this, FIRING was a dead-end state: nothing ever reverted it, so the UI would show
        // FIRING forever after a single shot. Lives here (the singleton), not on a Razor
        // page, so it still runs regardless of which page is open/navigated away from.
        private static readonly TimeSpan FireTimeout = TimeSpan.FromSeconds(20);
        private Timer? _fireTimeoutTimer;

        // פונקציה לעדכון המצב
        public void UpdateState(SystemState newState, string message)
        {
            CurrentState = newState;
            Logs.Insert(0, $"[{DateTime.Now:HH:mm:ss}] {message}");

            // מונע מהרשימה לגדול לנצח
            if (Logs.Count > 100) Logs.RemoveAt(100);

            if (newState == SystemState.FIRING)
            {
                // Each shot rolls the 20s window forward — only auto-closes once 20s
                // pass with NO further Fire() call.
                _fireTimeoutTimer?.Dispose();
                _fireTimeoutTimer = new Timer(OnFireTimeout, null, FireTimeout, Timeout.InfiniteTimeSpan);
            }
            else
            {
                // Any other explicit state change (SAFE/STANDBY/disarm/fault/...) cancels
                // the pending auto-revert — don't let a stale timer clobber it later.
                _fireTimeoutTimer?.Dispose();
                _fireTimeoutTimer = null;
            }

            // מודיע ל-Blazor לרענן את המסך
            NotifyStateChanged();
        }

        private void OnFireTimeout(object? state)
        {
            // Only revert if still FIRING — something else may have already changed
            // the state (and cancelled this timer) between scheduling and firing.
            if (CurrentState == SystemState.FIRING)
            {
                UpdateState(SystemState.ARMED, "FIRE window closed — no activity for 20s, reverted to ARMED");
            }
        }

        public void CameraOnCmd()
        {
            CameraOn = true;
            Logs.Insert(0, $"[{DateTime.Now:HH:mm:ss}] CAMERA: Micro300 ON");
            if (Logs.Count > 100) Logs.RemoveAt(100);
            NotifyStateChanged();
        }

        public void CameraOffCmd()
        {
            CameraOn = false;
            Logs.Insert(0, $"[{DateTime.Now:HH:mm:ss}] CAMERA: Micro300 OFF");
            if (Logs.Count > 100) Logs.RemoveAt(100);
            NotifyStateChanged();
        }

        public async Task InjectFccFreeze()
        {
            UpdateState(SystemState.STANDBY, "FI-004: Initiating FCC Freeze...");
            await Task.Delay(500); // המתנה לדימוי Timeout
            UpdateState(SystemState.FAULT, "WATCHDOG: Timeout Detected! Transitioning to SAFE.");
        }

        private void NotifyStateChanged() => OnChange?.Invoke();
    }
}
