/**
 * @file test_language.c
 * @brief Tests for the assistant language catalogue, locale resolution and prompt blocks
 */

#include <stdio.h>
#include <string.h>

#include "ethervox/language.h"

static int failures = 0;

#define CHECK(cond)                                                          \
    do {                                                                     \
        if (!(cond)) {                                                       \
            printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);           \
            failures++;                                                      \
        }                                                                    \
    } while (0)

static void expect_resolve(const char* tag, const char* want) {
    const char* got = ethervox_language_resolve(tag);
    int ok = want ? (got && strcmp(got, want) == 0) : (got == NULL);
    if (!ok) {
        printf("FAIL resolve(%s): want %s, got %s\n", tag ? tag : "NULL", want ? want : "NULL",
               got ? got : "NULL");
        failures++;
    }
}

int main(void) {
    // Catalogue: English first, every entry complete and unique
    CHECK(ethervox_language_count() >= 9);
    CHECK(strcmp(ethervox_language_at(0)->code, "en") == 0);
    CHECK(ethervox_language_at(ethervox_language_count()) == NULL);
    for (size_t i = 0; i < ethervox_language_count(); i++) {
        const ethervox_language_info_t* a = ethervox_language_at(i);
        CHECK(a && a->code && a->name_english && a->name_native);
        CHECK(strlen(a->code) == 2);
        for (size_t j = i + 1; j < ethervox_language_count(); j++) {
            CHECK(strcmp(a->code, ethervox_language_at(j)->code) != 0);
        }
    }
    CHECK(strcmp(ethervox_language_find("DE")->name_native, "Deutsch") == 0);
    CHECK(ethervox_language_find("xx") == NULL);
    CHECK(ethervox_language_find(NULL) == NULL);

    // Locale tags from every platform
    expect_resolve("de", "de");
    expect_resolve("de-AT", "de");
    expect_resolve("de_DE.UTF-8", "de");
    expect_resolve("DE-de", "de");
    expect_resolve("zh-Hans-CN", "zh");
    expect_resolve("pt-BR", "pt");
    expect_resolve("en_US", "en");
    expect_resolve("ja-JP", "ja");
    expect_resolve("sv-SE", NULL);
    expect_resolve("", NULL);
    expect_resolve("d", NULL);
    expect_resolve("deutsch", NULL);
    expect_resolve(NULL, NULL);

    // Prompt blocks: English is empty (base prompt is English); every other language has one that
    // keeps the tool-call format in English and is not just the English text
    CHECK(ethervox_language_prompt_block("en")[0] == '\0');
    CHECK(ethervox_language_prompt_block("xx")[0] == '\0');
    CHECK(ethervox_language_prompt_block(NULL)[0] == '\0');
    for (size_t i = 1; i < ethervox_language_count(); i++) {
        const char* code = ethervox_language_at(i)->code;
        const char* block = ethervox_language_prompt_block(code);
        CHECK(block[0] != '\0');
        CHECK(strlen(block) < 1024);
        CHECK(strstr(block, "Ethervox") != NULL);
        CHECK(strstr(block, "Always answer") == NULL);
    }
    CHECK(strstr(ethervox_language_prompt_block("de"), "Deutsch") != NULL);
    CHECK(strstr(ethervox_language_prompt_block("fr"), "français") != NULL);

    if (failures == 0) printf("test_language: all checks passed\n");
    return failures == 0 ? 0 : 1;
}
