/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request_charcheck.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bhazzout <bhazzout@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 17:41:10 by bhazzout          #+#    #+#             */
/*   Updated: 2024/02/01 17:41:11 by bhazzout         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.h"

bool isUnreservedCharacter(char ch) {
    // Check if the character is an unreserved character
    return (std::isalnum(ch) || ch == '-' || ch == '_' || ch == '.' || ch == '~');
}

bool isReservedCharacter(char ch) {
    // Check if the character is a reserved character
    return (ch == '!' || ch == '*' || ch == '\'' || ch == '(' || ch == ')' || ch == ';' || ch == ':' || ch == '@' ||
            ch == '&' || ch == '=' || ch == '+' || ch == '$' || ch == ',' || ch == '/' || ch == '?' || ch == '#');
}

bool containsNotAllowedCharacter(const std::string& uri) {
    for (size_t i = 0; i < uri.size(); i++) {
        // Check if the character is not allowed
        if (!isUnreservedCharacter(uri[i]) && !isReservedCharacter(uri[i])) {
            return true; // Found a not allowed character
        }
    }
    return false; // All characters are allowed
}