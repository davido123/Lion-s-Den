# Pre-Implementation Git Backup Script
# Run this script before starting Day 1 refactoring

Write-Host "=== Pre-Implementation Git Backup ===" -ForegroundColor Cyan
Write-Host ""

# Check if git is available
try {
    $gitVersion = git --version 2>&1
    Write-Host "Git found: $gitVersion" -ForegroundColor Green
} catch {
    Write-Host "ERROR: Git is not installed or not in PATH" -ForegroundColor Red
    exit 1
}

# Check if we're in a git repository
if (-not (Test-Path .git)) {
    Write-Host "Initializing new git repository..." -ForegroundColor Yellow
    git init
    Write-Host "Git repository initialized" -ForegroundColor Green
} else {
    Write-Host "Git repository found" -ForegroundColor Green
}

Write-Host ""
Write-Host "Current branch:" -ForegroundColor Cyan
git branch --show-current

Write-Host ""
Write-Host "Creating backup branch: backup-before-refactor" -ForegroundColor Yellow
git checkout -b backup-before-refactor 2>&1 | Out-Null

Write-Host "Staging all changes..." -ForegroundColor Yellow
git add .

Write-Host "Creating backup commit..." -ForegroundColor Yellow
git commit -m "Backup before game loop refactoring" 2>&1 | Out-Null

Write-Host ""
Write-Host "Creating working branch: refactor-game-loop" -ForegroundColor Yellow
git checkout -b refactor-game-loop 2>&1 | Out-Null

Write-Host ""
Write-Host "=== Git Backup Complete ===" -ForegroundColor Green
Write-Host ""
Write-Host "Current branches:" -ForegroundColor Cyan
git branch

Write-Host ""
Write-Host "You are now on branch: refactor-game-loop" -ForegroundColor Green
Write-Host "Backup branch: backup-before-refactor" -ForegroundColor Green
Write-Host ""
Write-Host "To rollback if needed:" -ForegroundColor Yellow
Write-Host "  git checkout backup-before-refactor" -ForegroundColor White
Write-Host ""
