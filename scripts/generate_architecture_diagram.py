import matplotlib.pyplot as plt
import matplotlib.patches as patches
from matplotlib.patches import FancyBboxPatch, ArrowStyle

def create_presentation_diagram():
    # Set Windows native font for full Polish diacritic support
    plt.rcParams['font.sans-serif'] = ['Segoe UI', 'Arial', 'DejaVu Sans']
    plt.rcParams['font.family'] = 'sans-serif'

    # 16:9 widescreen canvas, NO title at the top
    fig, ax = plt.subplots(figsize=(16, 9), dpi=300)
    ax.set_xlim(0, 100)
    ax.set_ylim(0, 100)
    ax.axis('off')
    fig.patch.set_facecolor('#FFFFFF')

    # Color palette
    c_ue_border = '#0284C7'       # Sky Blue
    c_ue_bg = '#F0F9FF'
    c_ue_header = '#0284C7'
    c_ue_inner = '#E0F2FE'

    c_rpi_border = '#16A34A'      # Emerald Green
    c_rpi_bg = '#F0FDF4'
    c_rpi_header = '#16A34A'
    c_rpi_inner = '#DCFCE7'

    c_vpn_border = '#7C3AED'      # Purple / Violet
    c_vpn_bg = '#F5F3FF'

    c_text_dark = '#0F172A'
    c_text_muted = '#475569'

    # Helper: Draw major entity container (UE or RPi5)
    def draw_container(x, y, w, h, border_color, bg_color, header_color, title):
        # Outer card
        card = FancyBboxPatch((x, y), w, h, boxstyle="round,pad=0.2,rounding_size=2.0",
                              facecolor=bg_color, edgecolor=border_color, linewidth=2.5, zorder=1)
        ax.add_patch(card)

        # Header bar (y=80 to y=90)
        hdr_h = 10.0
        hdr = FancyBboxPatch((x, y + h - hdr_h), w, hdr_h, boxstyle="round,pad=0.2,rounding_size=2.0",
                             facecolor=header_color, edgecolor=border_color, linewidth=2.0, zorder=2)
        ax.add_patch(hdr)

        # Header title
        ax.text(x + w/2, y + h - hdr_h/2, title, ha='center', va='center',
                fontsize=18, fontweight='bold', color='#FFFFFF', zorder=3)

    # Helper: Draw inner sub-block with clean bullets
    def draw_section_box(x, y, w, h, header_bg, border_color, sec_title, bullets):
        # Box background
        box = FancyBboxPatch((x, y), w, h, boxstyle="round,pad=0.15,rounding_size=1.2",
                             facecolor='#FFFFFF', edgecolor='#CBD5E1', linewidth=1.2, zorder=3)
        ax.add_patch(box)

        # Section header strip
        sec_h = 7.0
        sec_hdr = FancyBboxPatch((x, y + h - sec_h), w, sec_h, boxstyle="round,pad=0.15,rounding_size=1.2",
                                 facecolor=header_bg, edgecolor=border_color, linewidth=1.2, zorder=4)
        ax.add_patch(sec_hdr)

        # Section header text
        ax.text(x + 2.5, y + h - sec_h/2, sec_title, ha='left', va='center',
                fontsize=13.5, fontweight='bold', color=border_color, zorder=5)

        # Bullet points
        start_y = y + h - sec_h - 4.5
        spacing = (h - sec_h - 7.0) / max(len(bullets), 1)
        for i, text in enumerate(bullets):
            cur_y = start_y - i * spacing
            ax.text(x + 3.0, cur_y, "•", ha='left', va='center',
                    fontsize=16, fontweight='bold', color=border_color, zorder=5)
            ax.text(x + 5.5, cur_y, text, ha='left', va='center',
                    fontsize=12.5, fontweight='bold', color=c_text_dark, zorder=5)

    # ==========================================================
    # 1. LEFT CONTAINER: Urządzenie Użytkownika (UE) [x=3.0, w=36.5]
    # ==========================================================
    draw_container(x=3.0, y=10, w=36.5, h=80,
                   border_color=c_ue_border, bg_color=c_ue_bg, header_color=c_ue_header,
                   title="Urządzenie Użytkownika (UE)")

    # UE Section 1: Aplikacja kliencka (y=47 to 77, h=30)
    draw_section_box(x=5.2, y=47, w=32.0, h=30,
                     header_bg=c_ue_inner, border_color=c_ue_border,
                     sec_title="Aplikacja Kliencka (SmartSSH)",
                     bullets=[
                         "Połączenie SSH (zdalna konsola)",
                         "Zarządzanie kluczem (Ed25519)",
                         "Dostęp do plików i zasobów"
                     ])

    # UE Section 2: ZeroTier One (y=13 to 44, h=31)
    draw_section_box(x=5.2, y=13, w=32.0, h=31,
                     header_bg='#EDE9FE', border_color=c_vpn_border,
                     sec_title="ZeroTier One",
                     bullets=[
                         "Wirtualna sieć prywatna (VPN)",
                         "Warstwy L2 i L3 w sieci",
                         "Wirtualna karta sieciowa (zt0)"
                     ])

    # ==========================================================
    # 2. RIGHT CONTAINER: Serwer Raspberry Pi 5 [x=60.5, w=36.5]
    # ==========================================================
    draw_container(x=60.5, y=10, w=36.5, h=80,
                   border_color=c_rpi_border, bg_color=c_rpi_bg, header_color=c_rpi_header,
                   title="Serwer Raspberry Pi 5")

    # RPi5 Section 1: Zasoby i Usługi (y=47 to 77, h=30)
    draw_section_box(x=62.8, y=47, w=32.0, h=30,
                     header_bg=c_rpi_inner, border_color=c_rpi_border,
                     sec_title="Zasoby i Usługi Systemowe",
                     bullets=[
                         "Prywatny dysk sieciowy (Samba)",
                         "Wielodostęp użytkowników",
                         "Serwer OpenSSH (bezpieczny terminal)",
                         "Usługi Docker i monitoring zasobów"
                     ])

    # RPi5 Section 2: ZeroTier One (y=13 to 44, h=31)
    draw_section_box(x=62.8, y=13, w=32.0, h=31,
                     header_bg='#EDE9FE', border_color=c_vpn_border,
                     sec_title="ZeroTier One",
                     bullets=[
                         "Wirtualna sieć prywatna (VPN)",
                         "Warstwy L2 i L3 w sieci",
                         "Wirtualna karta sieciowa (zt0)"
                     ])

    # ==========================================================
    # 3. MIDDLE: JEDNA WYRAŹNA STRZAŁKA (SZYFROWANY TUNEL)
    # ==========================================================
    arrow_y = 45.0

    # Prominent single arrow pointing from UE to RPi 5
    arrow_style = ArrowStyle("->", head_length=8, head_width=5.5)
    tunnel_arrow = patches.FancyArrowPatch((39.8, arrow_y), (60.2, arrow_y),
                                           arrowstyle=arrow_style,
                                           color=c_vpn_border, linewidth=4.5, zorder=5)
    ax.add_patch(tunnel_arrow)

    # Info card placed cleanly over the arrow shaft
    card_w = 14.0
    card_h = 22.0
    card_x = 50.0 - card_w / 2  # x=43.0 to 57.0 (arrowhead at 60.2 is completely visible!)
    card_y = arrow_y - card_h / 2

    tunnel_card = FancyBboxPatch((card_x, card_y), card_w, card_h,
                                 boxstyle="round,pad=0.2,rounding_size=1.2",
                                 facecolor='#FFFFFF', edgecolor=c_vpn_border, linewidth=2.0, zorder=7)
    ax.add_patch(tunnel_card)

    # Header strip inside tunnel badge
    badge_hdr_h = 6.0
    badge_hdr = FancyBboxPatch((card_x, card_y + card_h - badge_hdr_h), card_w, badge_hdr_h,
                               boxstyle="round,pad=0.1,rounding_size=1.0",
                               facecolor=c_vpn_bg, edgecolor=c_vpn_border, linewidth=1.2, zorder=8)
    ax.add_patch(badge_hdr)

    # Tunnel Badge Text
    ax.text(50.0, card_y + card_h - badge_hdr_h/2, "Szyfrowany Tunel", ha='center', va='center',
            fontsize=11.5, fontweight='bold', color=c_vpn_border, zorder=9)

    ax.text(50.0, card_y + 11.2, "Szyfrowanie", ha='center', va='center',
            fontsize=11.0, fontweight='bold', color=c_text_dark, zorder=9)
    ax.text(50.0, card_y + 7.5, "End-to-End", ha='center', va='center',
            fontsize=10.5, fontweight='bold', color=c_vpn_border, zorder=9)
    ax.text(50.0, card_y + 3.5, "ChaCha20-Poly1305", ha='center', va='center',
            fontsize=8.5, family='monospace', color=c_text_muted, zorder=9)

    # Save to outputs
    plt.tight_layout()
    svg_path = 'docs/images/smartssh_architecture_diagram.svg'
    png_path = 'docs/images/smartssh_architecture_diagram.jpg'

    plt.savefig(svg_path, format='svg', bbox_inches='tight', pad_inches=0.05)
    plt.savefig(png_path, format='jpg', dpi=300, bbox_inches='tight', pad_inches=0.05)
    plt.close()
    print("Generated 2-box + 1-arrow diagram to:", svg_path, "and", png_path)

if __name__ == '__main__':
    create_presentation_diagram()
