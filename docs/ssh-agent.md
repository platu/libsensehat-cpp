# Raspberry Pi SSH client configuration

## GitHub identity

Specific SSH key generated for GitHub repository access.

```bash
ssh-keygen -t ed25519 -a 200 -f ~/.ssh/id_ed25519_github.pub

cat >> ~/.ssh/config << 'EOF'
AddKeysToAgent  yes

Host github.com
	HostName github.com
	IdentityFile ~/.ssh/id_ed25519_github.pub
EOF
```

## SSH agent setup

User level systemd unit for SSH agent

```bash
mkdir -p ~/.config/systemd/user/

cat > ~/.config/systemd/user/ssh-agent.service << 'EOF'
[Unit]
Description=SSH key agent

[Service]
Type=simple
Environment=SSH_AUTH_SOCK=%t/ssh-agent.socket
ExecStart=/usr/bin/ssh-agent -D -a $SSH_AUTH_SOCK

[Install]
WantedBy=default.target
EOF

systemctl --user enable ssh-agent

systemctl --user start ssh-agent

systemctl --user status ssh-agent
```

## SSH agent usage

```bash
echo "SSH_AUTH_SOCK DEFAULT=\"${XDG_RUNTIME_DIR}/ssh-agent.socket\"" > ~/.pam_environment
```

SSH agent socket access is available after logging out and logging in
