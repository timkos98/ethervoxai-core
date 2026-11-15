/**
 * @file language.h
 * @brief Assistant languages: catalogue, locale-tag resolution and translated system prompt blocks
 *
 * Every shell (Android, Apple, Windows, Linux, ESP32) shares this module, so the list of
 * supported languages, how an OS locale maps onto one of them and what the assistant is told
 * in each language are decided here once. A shell only supplies the OS locale tag and renders
 * its own interface strings.
 *
 * Copyright (c) 2024-2025 EthervoxAI Team
 * SPDX-License-Identifier: LicenseRef-EthervoxAI-Proprietary
 */

#ifndef ETHERVOX_LANGUAGE_H
#define ETHERVOX_LANGUAGE_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Code of the language used when nothing else matches. */
#define ETHERVOX_LANGUAGE_DEFAULT "en"

/** One language the assistant can answer in. All strings are static and UTF-8. */
typedef struct {
    const char* code;         /**< ISO 639-1 code, lower case ("de") */
    const char* name_english; /**< Name in English ("German") */
    const char* name_native;  /**< Name in the language itself ("Deutsch"), for language pickers */
} ethervox_language_info_t;

/** Number of supported languages. */
size_t ethervox_language_count(void);

/** The language at @p index (0 .. count-1, stable order, English first), or NULL if out of range. */
const ethervox_language_info_t* ethervox_language_at(size_t index);

/** The language with ISO code @p code (exact match, case-insensitive), or NULL. */
const ethervox_language_info_t* ethervox_language_find(const char* code);

/**
 * Map an OS locale tag onto a supported language code.
 *
 * Accepts BCP 47 and POSIX forms ("de", "de-AT", "de_DE.UTF-8", "zh-Hans-CN", "pt-BR"). Only
 * the primary language subtag is considered.
 *
 * @param tag Locale tag, may be NULL
 * @return A code from the catalogue, or NULL when the language is not supported
 *         (callers decide the fallback; ETHERVOX_LANGUAGE_DEFAULT is the usual one)
 */
const char* ethervox_language_resolve(const char* tag);

/**
 * The translated system prompt block for a language: who the assistant is, which language it
 * answers in, and that tool calls keep their English format. Shells never edit these; the
 * governor appends the block to its system prompt.
 *
 * @param code A catalogue code (anything else is treated as the default language)
 * @return A static string, empty for English (the base prompt is already English)
 */
const char* ethervox_language_prompt_block(const char* code);

#ifdef __cplusplus
}
#endif

#endif /* ETHERVOX_LANGUAGE_H */
