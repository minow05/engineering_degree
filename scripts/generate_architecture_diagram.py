import matplotlib.pyplot as plt
import matplotlib.patches as patches
from matplotlib.patches import FancyBboxPatch, ArrowStyle

def create_simple_presentation_diagram():
    # 16:9 widescreen presentation slide format, clean white background, NO title
    fig, ax = plt.subplots(figsize=(16, 9), dpi=300)
    ax.set_xlim(0, 100)
    ax.set_ylim(0, 100)
    ax.axis('off')
    fig.patch.set_facecolor('#FFFFFF')

    # Color palette - high contrast, clean slide styling
    c_ue_border = '#0284C7'       # Sky blue
    c_ue_bg = '#F0F9FF'
    c_ue_header = '#0284C7'
    
    c_vpn_border = '#7C3AED'      # Purple / Violet
    c_vpn_bg = '#F5F3FF'
    c_vpn_header = '#7C3AED'
    
    c_rpi_border = '#16A34A'      # Green
    c_rpi_bg = '#F0FDF4'
    c_rpi_header = '#16A34A'

    c_text_dark = '#0F172A'

    def draw_card(x, y, w, h, border_color, bg_color, header_color, title, subtitle, bullets):
        # Outer card background
        card = FancyBboxPatch((x, y), w, h, boxstyle="round,pad=0.2,rounding_size=2.0",
                              facecolor=bg_color, edgecolor=border_color, linewidth=2.5, zorder=1)
        ax.add_patch(card)

        # Header block
        hdr_h = 13.0
        hdr = FancyBboxPatch((x, y + h - hdr_h), w, hdr_h, boxstyle="round,pad=0.2,rounding_size=2.0",
                             facecolor=header_color, edgecolor=border_color, linewidth=2.0, zorder=2)
        ax.add_patch(hdr)

        # Header texts
        ax.text(x + w/2, y + h - 5.0, title, ha='center', va='center',
                fontsize=16.5, fontweight='bold', color='#FFFFFF', zorder=3)
        if subtitle:
            ax.text(x + w/2, y + h - 9.5, subtitle, ha='center', va='center',
                    fontsize=12, color='#F1F5F9', zorder=3)

        # Inner white card for clean contrast
        inner_m = 1.6
        inner_w = w - 2 * inner_m
        inner_h = h - hdr_h - 2.5 * inner_m
        inner = FancyBboxPatch((x + inner_m, y + inner_m), inner_w, inner_h,
                               boxstyle="round,pad=0.2,rounding_size=1.2",
                               facecolor='#FFFFFF', edgecolor='#CBD5E1', linewidth=1.2, zorder=3)
        ax.add_patch(inner)

        # Bullet points - single line each, well-fitted font size
        start_y = y + inner_m + inner_h - 7.5
        spacing = (inner_h - 15.0) / max(len(bullets) - 1, 1)
        for i, text in enumerate(bullets):
            cur_y = start_y - i * spacing
            # Bullet point marker
            ax.text(x + inner_m + 2.2, cur_y, "•", ha='left', va='center',
                    fontsize=18, fontweight='bold', color=border_color, zorder=4)
            # Bullet text
            ax.text(x + inner_m + 4.8, cur_y, text, ha='left', va='center',
                    fontsize=12.2, fontweight='bold', color=c_text_dark, zorder=4)

    # ==========================================================
    # 3 CARDS: ONLY BULLET POINTS, MINIMAL TEXT, PERFECT PADDING
    # ==========================================================

    # CARD 1: Urządzenie Użytkownika (UE)
    draw_card(
        x=3.0, y=14, w=28.5, h=72,
        border_color=c_ue_border, bg_color=c_ue_bg, header_color=c_ue_header,
        title="Urządzenie Użytkownika (UE)",
        subtitle="Laptop, PC lub Smartfon",
        bullets=[
            "Aplikacja SmartSSH",
            "Klucz SSH (Ed25519)",
            "Zdalna konsola i pliki",
            "Wirtualna karta VPN"
        ]
    )

    # CARD 2: Bezpieczna Sieć P2P (ZeroTier VPN)
    draw_card(
        x=35.5, y=14, w=28.5, h=72,
        border_color=c_vpn_border, bg_color=c_vpn_bg, header_color=c_vpn_header,
        title="Szyfrowany Tunel (VPN)",
        subtitle="ZeroTier P2P • Internet",
        bullets=[
            "Szyfrowany tunel w sieci",
            "Dostęp z dowolnego miejsca",
            "SSH (Port 22) – terminal",
            "Samba (Port 445) – pliki"
        ]
    )

    # CARD 3: Serwer Raspberry Pi 5
    draw_card(
        x=68.0, y=14, w=28.5, h=72,
        border_color=c_rpi_border, bg_color=c_rpi_bg, header_color=c_rpi_header,
        title="Serwer Raspberry Pi 5",
        subtitle="Centrum Danych i Usług",
        bullets=[
            "Mikrokomputer RPi 5",
            "Prywatny dysk sieciowy",
            "Usługi i Docker",
            "Wielodostęp użytkowników"
        ]
    )

    # ==========================================================
    # ARROWS WITH BADGES
    # ==========================================================
    def draw_slide_arrow(x1, x2, y, label, color):
        arrow_style = ArrowStyle("->", head_length=6, head_width=4.0)
        patch = patches.FancyArrowPatch((x1, y), (x2, y),
                                        arrowstyle=arrow_style,
                                        color=color, linewidth=2.8, zorder=5)
        ax.add_patch(patch)
        mid_x = (x1 + x2) / 2
        ax.text(mid_x, y + 2.2, label, ha='center', va='bottom',
                fontsize=10.5, fontweight='bold', color=color, zorder=6,
                bbox=dict(boxstyle="round,pad=0.25", facecolor="#FFFFFF", edgecolor=color, linewidth=1.3))

    # Gap 1: x=31.5 to 35.5
    draw_slide_arrow(31.5, 35.5, 50, "Tunel P2P", c_ue_border)

    # Gap 2: x=64.0 to 68.0
    draw_slide_arrow(64.0, 68.0, 50, "SSH & Samba", c_rpi_border)

    # Save to outputs
    plt.tight_layout()
    svg_path = 'docs/images/smartssh_architecture_diagram.svg'
    png_path = 'docs/images/smartssh_architecture_diagram.jpg'
    
    plt.savefig(svg_path, format='svg', bbox_inches='tight', pad_inches=0.05)
    plt.savefig(png_path, format='jpg', dpi=300, bbox_inches='tight', pad_inches=0.05)
    plt.close()
    print("Generated ultra-simple presentation diagram to:", svg_path, "and", png_path)

if __name__ == '__main__':
    create_simple_presentation_diagram()
