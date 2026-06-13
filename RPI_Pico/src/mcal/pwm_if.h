#ifndef PWM_IF_H
#define PWM_IF_H

/* PWM registers */

#define x_PWM_BASE                      0x40050000
#define x_PWM_CSR                       (x_PWM_BASE + 0x00)
#define x_PWM_DIV                       (x_PWM_BASE + 0x04)
#define x_PWM_CTR                       (x_PWM_BASE + 0x08)
#define x_PWM_CC                        (x_PWM_BASE + 0x0C)
#define x_PWM_TOP                       (x_PWM_BASE + 0x10)
#define x_PWM_CHANNEL_x_OFFSET          0x14

/* PWM special bits */

#define x_PWM_CSR_EN                    0x01
#define x_PWM_PHASE_CORRECT             0x02
#define x_PWM_DIV_INT_OFFSET            0x04
#define x_PWM_DIV_FRAC_OFFSET           0x00
#define x_PWM_CC_A_OFFSET               0x00
#define x_PWM_CC_B_OFFSET               0x0F

/* PWM interfaces */

#endif /* PWM_IF_H */