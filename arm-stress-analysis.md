# Drone Arm Structural Analysis — Cantilever Bending Study

*Draft for portfolio — hand calculation of arm bending under flight thrust*

## Purpose

This analysis checks whether a single arm of the quadcopter frame is strong enough
to survive the thrust load it carries in flight, and how much it flexes while doing so.
Because the free (non-commercial) Autodesk Fusion licence does not include the
Simulation workspace, the analysis was done **by hand** using classical beam theory,
rather than with finite-element software. Working it by hand also makes the underlying
mechanics explicit rather than hidden inside a solver.

The study also compares the current **10 mm-thick** arm against a hypothetical
**12 mm-thick** arm, to quantify the design trade-off between stiffness and weight.

## The engineering model

Each arm is treated as a **cantilever beam**: rigidly fixed at the hub end (where it
bolts to the central plate) and loaded at the free end by the motor's thrust pulling
upward. A cantilever is most highly stressed at its fixed end, so the arm root (at the
hub) is the critical section and is used for the stress calculation.

```
        Fixed at hub                    Thrust F (up)
             |                                |
             v                                v
        [====================================]  <-- arm
        ^                                    ^
        root (max stress here)            motor mount
        |<-------------- L --------------->|
```

## Inputs

| Quantity | Symbol | Value | Source |
|---|---|---|---|
| Thrust per motor (full throttle) | F | 8.3 N (≈ 850 g) | Representative for A2212 1000KV + 1045 prop on 3S |
| Effective beam length (hub to motor) | L | 0.180 m | From CAD geometry |
| Arm root width | b | 0.022 m | Measured in CAD |
| Arm root thickness (bending direction) | h | 0.010 m | Measured in CAD |
| ABS yield strength (approx.) | σ_yield | 40 MPa | Representative bulk value |
| ABS elastic modulus (approx.) | E | 2.3 GPa | Representative bulk value |

The arm bends **vertically** (thrust pulls up), so the dimension that resists bending is
the 10 mm thickness, not the 22 mm width. This matters a great deal — see the results.

## Method and calculations (10 mm arm)

### Step 1 — Bending moment at the root

The thrust at the tip creates a bending moment at the fixed root:

```
M = F × L = 8.3 × 0.180 = 1.49 N·m
```

### Step 2 — Second moment of area

The cross-section's resistance to bending is its second moment of area. For a rectangle
bending about its width axis:

```
I = (b × h³) / 12
I = (0.022 × 0.010³) / 12
I = 1.83 × 10⁻⁹ m⁴
```

The thickness `h` is **cubed**, so it dominates the result — this is the single most
important feature of the whole analysis.

### Step 3 — Maximum bending stress

Peak stress occurs at the surface furthest from the neutral axis, at distance
c = h/2 = 0.005 m:

```
σ = (M × c) / I
σ = (1.49 × 0.005) / (1.83 × 10⁻⁹)
σ ≈ 4.1 MPa
```

### Step 4 — Safety factor

```
SF = σ_yield / σ = 40 / 4.1 ≈ 9.8
```

### Step 5 — Tip deflection

```
δ = (F × L³) / (3 × E × I)
δ = (8.3 × 0.180³) / (3 × 2.3×10⁹ × 1.83×10⁻⁹)
δ ≈ 3.8 mm
```

## Results — 10 mm vs 12 mm arm

The calculation was repeated with the thickness increased to 12 mm (all other inputs
unchanged) to quantify the effect of a modest thickness increase.

| Quantity | 10 mm arm | 12 mm arm | Change |
|---|---|---|---|
| Second moment of area, I | 1.83 × 10⁻⁹ m⁴ | 3.17 × 10⁻⁹ m⁴ | +73 % |
| Max bending stress, σ | 4.1 MPa | 2.8 MPa | −32 % |
| Safety factor, SF | ≈ 9.8 | ≈ 14.3 | stronger |
| Tip deflection, δ | 3.8 mm | 2.2 mm | −42 % |
| Relative arm material / weight | baseline | +20 % | heavier |

## Interpretation

Two separate conclusions come out of this, and they are worth keeping distinct because
**strength and stiffness are different things**:

- **Strength:** Both arms pass comfortably. Even the 10 mm arm has a safety factor of
  roughly 10 against the flight thrust load, meaning it could carry nearly ten times the
  full-thrust force before the material begins to yield. Increasing thickness raises this
  further, but it was never the limiting factor.

- **Stiffness:** The 10 mm arm deflects about 3.8 mm at the motor tip under full thrust.
  This is not dangerous, but it is enough that the arms would flex slightly during
  aggressive flight. The 12 mm arm cuts this flex by 42 %, to 2.2 mm.

A 20 % increase in thickness (10 → 12 mm) produces a 73 % increase in bending resistance,
because the thickness enters the second moment of area **cubed**. This is a large return
for a small dimensional change.

## Design decision

**The 10 mm arm was retained.** The reasoning:

- It passes the strength check with a large margin.
- The 3.8 mm tip flex is acceptable for gentle, learning-stage flying.
- Weight is the primary enemy of flight time and thrust-to-weight ratio, and the 12 mm
  arm costs 20 % more material per arm across four arms.

If, in flight testing, the arms are found to flex too much for comfort, the fix is a
single parameter change (arm thickness), and the cube relationship means a small increase
will have a disproportionately large stiffening effect. The analysis makes that decision
informed and reversible.

## Key structural principle

To make a beam stiffer, **add depth in the bending direction, not width** — because depth
is cubed in the second moment of area while width is only linear. A 12 × 22 mm arm is far
stiffer in the bending direction than a 10 × 26 mm arm of similar weight. This is the same
reason floor joists, I-beams, and aircraft wing spars are built tall rather than wide.

## Limitations and assumptions

This is a first-order hand calculation, and its approximations should be stated honestly:

- **Constant cross-section assumed.** The real arm tapers from 22 mm at the hub to 40 mm
  at the motor end. Using the narrower root section makes the calculation *conservative*
  (the real arm is stronger toward the tip than modelled).
- **Lightening holes ignored.** The holes create local stress concentrations that a simple
  beam calculation cannot capture, so the true peak stress at a hole edge is somewhat
  higher than 4.1 MPa. The large safety factor (~10) provides ample room to absorb this.
- **Bulk material properties used.** The 40 MPa yield strength and 2.3 GPa modulus are for
  solid ABS. FDM-printed ABS is weaker, particularly across layer lines (the Z direction),
  and is typically printed with partial infill. Real printed strength may be roughly
  50–70 % of the bulk figure depending on print orientation and infill, so the effective
  safety factor is lower than 10 — but still comfortably above 1.
- **Static load only.** This analysis covers steady thrust. It does not cover impact
  (hard landings) or vibration fatigue, which would be separate studies.

Despite these simplifications, the analysis gives a clear, defensible pass/fail result and
a genuine design insight, which is its purpose.
