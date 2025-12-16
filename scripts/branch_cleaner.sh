#!/bin/bash

# Hent alle lokale branches
branches=($(git branch --format='%(refname:short)'))

# Sett master først, resten sortert alfabetisk
branches_sorted=()
for b in "${branches[@]}"; do
    if [ "$b" == "master" ]; then
        branches_sorted=("master" "${branches_sorted[@]}")
    else
        branches_sorted+=("$b")
    fi
done

while true; do
    echo "Local branches:"
    for i in "${!branches_sorted[@]}"; do
        printf "%d: %s\n" "$((i+1))" "${branches_sorted[$i]}"
    done

    read -p "Enter number of branch to delete (or 0 to quit): " choice
    if [ "$choice" -eq 0 ]; then
        echo "Exiting."
        break
    fi

    if [ "$choice" -ge 1 ] && [ "$choice" -le "${#branches_sorted[@]}" ]; then
        branch_to_delete="${branches_sorted[$((choice-1))]}"
        # Sjekk om master
        if [ "$branch_to_delete" == "master" ]; then
            echo "Cannot delete master branch!"
            continue
        fi

        # Slett lokalt
        git branch -d "$branch_to_delete"

        # Spør om remote
        read -p "Delete remote branch too? (y/N): " del_remote
        if [[ "$del_remote" =~ ^[Yy]$ ]]; then
            git push origin --delete "$branch_to_delete"
        fi

        # Fjern fra listen
        unset branches_sorted[$((choice-1))]
        branches_sorted=("${branches_sorted[@]}")  # reindex
    else
        echo "Invalid choice, try again."
    fi
done
