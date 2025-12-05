#!/bin/bash

# Hent lokale branches (master øverst)
branches=($(git branch --format='%(refname:short)'))
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

    read -p "Enter number of branch to merge master into (or 0 to quit): " choice
    if [ "$choice" -eq 0 ]; then
        echo "Exiting."
        break
    fi

    if [ "$choice" -ge 1 ] && [ "$choice" -le "${#branches_sorted[@]}" ]; then
        branch="${branches_sorted[$((choice-1))]}"
        if [ "$branch" == "master" ]; then
            echo "Cannot merge master into itself!"
            continue
        fi

        echo "Checking out $branch..."
        git checkout "$branch"
        echo "Merging master into $branch..."
        git merge master
        echo "Done merging master into $branch."
    else
        echo "Invalid choice, try again."
    fi
done
