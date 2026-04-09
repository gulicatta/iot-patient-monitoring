import {
  Box,
  Chip,
  Collapse,
  Divider,
  Paper,
  Typography,
} from "@mui/material";
import ThermostatIcon from "@mui/icons-material/Thermostat";
import WaterDropIcon from "@mui/icons-material/WaterDrop";
import WarningAmberIcon from "@mui/icons-material/WarningAmber";
import VitalCard from "./VitalCard";
import PillTimer from "./PillTimer";

function SectionDivider({ label }) {
  return (
    <Divider sx={{ my: 2.5 }}>
      <Typography variant="overline" color="text.secondary" sx={{ fontSize: "0.7rem", letterSpacing: 1 }}>
        {label}
      </Typography>
    </Divider>
  );
}

function tempColor(temp, thresholds) {
  if (temp == null || !thresholds) return "info.main";
  if (temp >= thresholds.alarm_threshold) return "error.main";
  if (temp >= thresholds.warning_threshold) return "warning.main";
  return "success.main";
}

export default function PatientDetail({ did, detail, pillStatus, onConfirmPill, tempThresholds }) {
  return (
    <Collapse in={did !== null} unmountOnExit>
      <Paper sx={{ p: 3, mt: 2 }}>
        <Box sx={{ display: "flex", alignItems: "baseline", gap: 2, mb: 1 }}>
          <Typography variant="h6" fontWeight={600} sx={{ fontFamily: "monospace" }}>
            {did}
          </Typography>
          {detail?.ts && (
            <Typography variant="caption" color="text.secondary">
              Last update: {new Date(detail.ts).toLocaleTimeString()}
            </Typography>
          )}
        </Box>

        <SectionDivider label="Vital Signs" />

        <Box sx={{ display: "flex", gap: 2, flexWrap: "wrap" }}>
          <VitalCard
            icon={<ThermostatIcon fontSize="inherit" />}
            label="Temperature"
            unit="°C"
            value={detail?.temp}
            color={tempColor(detail?.temp, tempThresholds)}
          />
          <VitalCard
            icon={<WaterDropIcon fontSize="inherit" />}
            label="Humidity"
            unit="%"
            value={detail?.hum}
            color="info.main"
          />
        </Box>

        <SectionDivider label="State" />

        <Box sx={{ display: "flex", gap: 1.5, flexWrap: "wrap", alignItems: "center" }}>
          {detail?.alarm
            ? <Chip icon={<WarningAmberIcon />} label="ALARM" color="error" size="small" />
            : <Chip label="No Alarm" color="success" size="small" variant="outlined" />
          }
        </Box>

        <SectionDivider label="Pill Schedule" />

        <PillTimer did={did} status={pillStatus} onConfirm={onConfirmPill} />
      </Paper>
    </Collapse>
  );
}
