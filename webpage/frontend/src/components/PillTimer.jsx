import { Box, Button, Chip, Paper, Typography } from "@mui/material";
import MedicationIcon from "@mui/icons-material/Medication";

function formatDuration(minutes) {
  const h = Math.floor(Math.abs(minutes) / 60);
  const m = (Math.abs(minutes)).toFixed(1);
  return h > 0 ? `${h}h ${m}min` : `${m} min`;
}

function pillChip(status) {
  if (!status) return null;

  if (status.overdue) {
    const label = status.last_confirmed
      ? `Overdue by ${formatDuration(status.minutes_remaining)}`
      : "Never confirmed";
    return <Chip icon={<MedicationIcon />} label={label} color="error" size="small" />;
  }

  const min = status.minutes_remaining?.toFixed(2) ?? 0;
  const label =
    min <= 60
      ? `Due in ${formatDuration(min)}`
      : `Next in ${formatDuration(min)}`;
  return (
    <Chip
      icon={<MedicationIcon />}
      label={label}
      color={min <= 60 ? "warning" : "success"}
      size="small"
    />
  );
}

export default function PillTimer({ did, status, onConfirm }) {
  const chip = pillChip(status);

  return (
    <Paper variant="outlined" sx={{ p: 1.5, display: "flex", alignItems: "center", gap: 2, flexWrap: "wrap" }}>
      {chip ?? (
        <Chip label="Status unavailable" size="small" variant="outlined" />
      )}

      {status?.last_confirmed && (
        <Typography variant="caption" color="text.secondary">
          Last taken: {new Date(status.last_confirmed).toLocaleTimeString()}
        </Typography>
      )}

      <Button
        variant="outlined"
        size="small"
        onClick={() => onConfirm(did)}
        sx={{ ml: "auto" }}
      >
        Mark as taken
      </Button>
    </Paper>
  );
}
