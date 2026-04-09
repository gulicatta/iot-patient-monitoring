import { alpha } from "@mui/material/styles";
import { Box, Card, CardContent, Typography } from "@mui/material";

export default function VitalCard({ icon, label, value, unit, color }) {
  return (
    <Card
      variant="outlined"
      sx={{
        flex: 1,
        minWidth: 150,
        borderLeft: `4px solid`,
        borderLeftColor: color,
        borderTop: "1px solid #e2e8f0",
        borderRight: "1px solid #e2e8f0",
        borderBottom: "1px solid #e2e8f0",
      }}
    >
      <CardContent sx={{ display: "flex", alignItems: "center", gap: 2, py: "14px !important" }}>
        <Box
          sx={{
            width: 44,
            height: 44,
            borderRadius: "50%",
            backgroundColor: (theme) => alpha(theme.palette[color.split(".")[0]]?.main ?? color, 0.12),
            display: "flex",
            alignItems: "center",
            justifyContent: "center",
            color,
            fontSize: 22,
            flexShrink: 0,
          }}
        >
          {icon}
        </Box>
        <Box>
          <Typography variant="h6" fontWeight={700} lineHeight={1.1}>
            {value != null ? value.toFixed(2) : "—"}
            <Typography component="span" variant="caption" color="text.secondary" sx={{ ml: 0.5 }}>
              {unit}
            </Typography>
          </Typography>
          <Typography variant="body2" color="text.secondary" sx={{ mt: 0.25 }}>
            {label}
          </Typography>
        </Box>
      </CardContent>
    </Card>
  );
}
