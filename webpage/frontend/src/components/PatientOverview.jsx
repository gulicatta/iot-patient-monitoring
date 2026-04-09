import {
  Avatar,
  Box,
  Chip,
  Paper,
  Table,
  TableBody,
  TableCell,
  TableContainer,
  TableHead,
  TableRow,
  Typography,
} from "@mui/material";
import MedicationIcon from "@mui/icons-material/Medication";
import WarningAmberIcon from "@mui/icons-material/WarningAmber";

function fmt(value, digits) {
  return value != null ? value.toFixed(digits) : "—";
}

function StatusDot({ lastSeen }) {
  if (!lastSeen) return <Box component="span" sx={{ color: "text.disabled" }}>— unknown</Box>;
  const ageMs = Date.now() - new Date(lastSeen).getTime();
  if (ageMs < 5000)  return <Box component="span" sx={{ color: "success.main" }}>● Live</Box>;
  if (ageMs < 30000) return <Box component="span" sx={{ color: "warning.main" }}>● Recent</Box>;
  return <Box component="span" sx={{ color: "error.main" }}>● Stale</Box>;
}

function pillChip(status) {
  if (!status) return <Chip label="—" size="small" variant="outlined" />;
  if (status.overdue) return <Chip icon={<MedicationIcon />} label="Overdue" color="error" size="small" />;
  const min = status.minutes_remaining?.toFixed(1) ?? 0;
  if (min <= 60) return <Chip icon={<MedicationIcon />} label={`${min} min`} color="warning" size="small" />;
  const h = Math.floor(min / 60);
  return <Chip icon={<MedicationIcon />} label={`${h}h`} color="success" size="small" />;
}

function tempColor(temp, thresholds) {
  if (temp == null || !thresholds) return "inherit";
  if (temp >= thresholds.alarm_threshold) return "error.main";
  if (temp >= thresholds.warning_threshold) return "warning.main";
  return "inherit";
}

export default function PatientOverview({ patients, selectedDid, onSelect, pillStatuses, tempThresholds }) {
  return (
    <Paper>
      <Box sx={{ px: 2.5, py: 2, display: "flex", alignItems: "baseline", gap: 1.5 }}>
        <Typography variant="subtitle1" fontWeight={600}>
          Patient Overview
        </Typography>
        <Typography variant="body2" color="text.secondary">
          {patients.length} patient{patients.length !== 1 ? "s" : ""}
        </Typography>
      </Box>
      <TableContainer>
        <Table>
          <TableHead>
            <TableRow sx={{ "& th": { fontWeight: 600, color: "text.secondary", fontSize: "0.75rem", letterSpacing: 0.5 } }}>
              <TableCell>Device ID</TableCell>
              <TableCell>Last Seen</TableCell>
              <TableCell align="right">Temp (°C)</TableCell>
              <TableCell align="right">Humidity (%)</TableCell>
              <TableCell align="center">Alarm</TableCell>
              <TableCell align="left">Status</TableCell>
              <TableCell align="center">Pill</TableCell>
            </TableRow>
          </TableHead>
          <TableBody>
            {patients.length === 0 && (
              <TableRow>
                <TableCell colSpan={7}>
                  <Typography color="text.secondary" sx={{ py: 2, textAlign: "center" }}>
                    No patients yet…
                  </Typography>
                </TableCell>
              </TableRow>
            )}
            {patients.map((p) => {
              const v = p.latest ?? {};
              const selected = p.id === selectedDid;
              return (
                <TableRow
                  key={p.id}
                  hover
                  selected={selected}
                  onClick={() => onSelect(p.id === selectedDid ? null : p.id)}
                  sx={{ cursor: "pointer",backgroundColor: v.alarm ? "orange" : "white" }}
                >
                  <TableCell>
                    <Box sx={{ display: "flex", alignItems: "center", gap: 1.5 }}>
                      <Avatar
                        sx={{
                          width: 32,
                          height: 32,
                          fontSize: "0.7rem",
                          fontWeight: 700,
                          bgcolor: "primary.main",
                        }}
                      >
                        {p.id?.slice(-4)}
                      </Avatar>
                      <Typography variant="body2" fontWeight={500} sx={{ fontFamily: "monospace" }}>
                        {p.id}
                      </Typography>
                    </Box>
                  </TableCell>
                  <TableCell>
                    <Typography variant="body2" color="text.secondary">
                      {p.last_seen ? new Date(p.last_seen).toLocaleTimeString() : "—"}
                    </Typography>
                  </TableCell>
                  <TableCell align="right">
                    <Box component="span" sx={{ color: tempColor(v.temp, tempThresholds), fontWeight: v.temp != null && tempThresholds && v.temp >= tempThresholds.warning_threshold ? 700 : "inherit" }}>
                      {fmt(v.temp, 2)}
                    </Box>
                  </TableCell>
                  <TableCell align="right">{fmt(v.hum, 2)}</TableCell>
                  <TableCell align="center">
                    {v.alarm
                      ? <WarningAmberIcon fontSize="small" color="error" />
                      : <Typography variant="body2" color="text.disabled">—</Typography>
                    }
                  </TableCell>
                  <TableCell>
                    <Typography variant="body2" fontWeight={500}>
                      <StatusDot lastSeen={p.last_seen} />
                    </Typography>
                  </TableCell>
                  <TableCell align="center">{pillChip(pillStatuses?.[p.id])}</TableCell>
                </TableRow>
              );
            })}
          </TableBody>
        </Table>
      </TableContainer>
    </Paper>
  );
}
