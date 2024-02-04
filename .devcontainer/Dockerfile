# Dockerfile
FROM mcr.microsoft.com/devcontainers/rust:1-1-bullseye

# Install additional dependencies if needed
# RUN apt-get update && apt-get install -y <package_name>

# Copy project into container
COPY . .

# Install Taskfile.dev
RUN sh -c "$(curl --location https://taskfile.dev/install.sh)" -- -d -b /usr/local/bin

# Run the post-create command
RUN rustc --version
