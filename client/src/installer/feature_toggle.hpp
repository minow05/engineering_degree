#pragma once

namespace smart_ssh::installer {

struct feature_flags {
    bool ssh_keys{true};
    bool samba_storage{true};
    bool process_hosting{true};
};

} // namespace smart_ssh::installer
