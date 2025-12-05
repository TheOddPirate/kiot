#!/bin/bash

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

    read -p "Enter number of branch to rebase onto master (or 0 to quit): " choice
    if [ "$choice" -eq 0 ]; then
        echo "Exiting."
        break
    fi

    if [ "$choice" -ge 1 ] && [ "$choice" -le "${#branches_sorted[@]}" ]; then
        branch="${branches_sorted[$((choice-1))]}"
        if [ "$branch" == "master" ]; then
            echo "Cannot rebase master onto itself!"
            continue
        fi

        echo "Checking out $branch..."
        git checkout "$branch"
        echo "Rebasing $branch onto master..."
        git rebase master
        echo "Done rebasing $branch onto master."
    else
        echo "Invalid choice, try again."
    fi
done
