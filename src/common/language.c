/**
 * @file language.c
 * @brief Assistant language catalogue, locale resolution and translated prompt blocks
 *
 * Copyright (c) 2024-2025 EthervoxAI Team
 * SPDX-License-Identifier: LicenseRef-EthervoxAI-Proprietary
 */

#include "ethervox/language.h"

#include <ctype.h>
#include <string.h>

/*
 * The prompt block is written in the target language: small models follow an instruction in the
 * language they must answer in far more reliably than an English instruction asking for it. Tool
 * calls stay in the English format defined by the base prompt, so only explanations change.
 * English is empty: the base prompt already is English, which keeps existing English prefill
 * caches valid.
 */
typedef struct {
    ethervox_language_info_t info;
    const char* prompt_block;
} language_entry_t;

static const language_entry_t kLanguages[] = {
    {{"en", "English", "English"}, ""},
    {{"de", "German", "Deutsch"},
     "Du bist Ethervox, ein freundlicher Assistent, der privat auf dem Gerät des Nutzers läuft. "
     "Antworte immer auf Deutsch, kurz und hilfreich, auch wenn die Frage kurz ist oder mehrere "
     "Sprachen mischt – außer der Nutzer bittet um eine andere Sprache. Wenn ein Werkzeug nötig "
     "ist, rufe es exakt im oben beschriebenen englischen Format auf und erkläre das Ergebnis "
     "danach auf Deutsch."},
    {{"es", "Spanish", "Español"},
     "Eres Ethervox, un asistente amable que funciona de forma privada en el dispositivo del "
     "usuario. Responde siempre en español, de forma breve y útil, incluso si la pregunta es "
     "corta o mezcla idiomas, salvo que el usuario pida otro idioma. Cuando necesites una "
     "herramienta, llámala exactamente con el formato en inglés descrito arriba y luego explica "
     "el resultado en español."},
    {{"fr", "French", "Français"},
     "Tu es Ethervox, un assistant aimable qui fonctionne de manière privée sur l'appareil de "
     "l'utilisateur. Réponds toujours en français, de façon brève et utile, même si la question "
     "est courte ou mélange plusieurs langues, sauf si l'utilisateur demande une autre langue. "
     "Quand un outil est nécessaire, appelle-le exactement dans le format anglais décrit "
     "ci-dessus, puis explique le résultat en français."},
    {{"it", "Italian", "Italiano"},
     "Sei Ethervox, un assistente cordiale che funziona in modo privato sul dispositivo "
     "dell'utente. Rispondi sempre in italiano, in modo breve e utile, anche se la domanda è "
     "corta o mescola più lingue, a meno che l'utente chieda un'altra lingua. Quando serve uno "
     "strumento, richiamalo esattamente nel formato inglese descritto sopra e poi spiega il "
     "risultato in italiano."},
    {{"pt", "Portuguese", "Português"},
     "Você é o Ethervox, um assistente simpático que funciona de forma privada no dispositivo do "
     "usuário. Responda sempre em português, de forma breve e útil, mesmo que a pergunta seja "
     "curta ou misture idiomas, a menos que o usuário peça outro idioma. Quando precisar de uma "
     "ferramenta, chame-a exatamente no formato em inglês descrito acima e depois explique o "
     "resultado em português."},
    {{"zh", "Chinese", "中文"},
     "你是 Ethervox，一个在用户设备上私密运行的友好助手。请始终使用简体中文回答，简洁且有帮助，"
     "即使问题很短或混用多种语言，除非用户要求使用其他语言。需要使用工具时，请严格按照上文所述的"
     "英文格式调用工具，然后用中文解释结果。"},
    {{"ja", "Japanese", "日本語"},
     "あなたは Ethervox です。ユーザーの端末上でプライベートに動作する親切なアシスタントです。"
     "質問が短い場合や複数の言語が混ざっている場合でも、ユーザーが別の言語を求めない限り、常に"
     "日本語で簡潔かつ役立つ回答をしてください。ツールが必要な場合は、上記の英語の形式のとおりに"
     "正確に呼び出し、その結果を日本語で説明してください。"},
    {{"ko", "Korean", "한국어"},
     "당신은 Ethervox이며, 사용자의 기기에서 비공개로 동작하는 친절한 어시스턴트입니다. 질문이 "
     "짧거나 여러 언어가 섞여 있어도, 사용자가 다른 언어를 요청하지 않는 한 항상 한국어로 "
     "간결하고 도움이 되게 답하세요. 도구가 필요하면 위에 설명된 영어 형식 그대로 정확히 "
     "호출한 뒤, 결과를 한국어로 설명하세요."},
};

#define LANGUAGE_COUNT (sizeof(kLanguages) / sizeof(kLanguages[0]))

static const language_entry_t* find_entry(const char* code) {
    if (!code) return NULL;
    for (size_t i = 0; i < LANGUAGE_COUNT; i++) {
        const char* a = kLanguages[i].info.code;
        const char* b = code;
        while (*a && *b && tolower((unsigned char)*a) == tolower((unsigned char)*b)) {
            a++;
            b++;
        }
        if (*a == '\0' && *b == '\0') return &kLanguages[i];
    }
    return NULL;
}

size_t ethervox_language_count(void) { return LANGUAGE_COUNT; }

const ethervox_language_info_t* ethervox_language_at(size_t index) {
    return index < LANGUAGE_COUNT ? &kLanguages[index].info : NULL;
}

const ethervox_language_info_t* ethervox_language_find(const char* code) {
    const language_entry_t* entry = find_entry(code);
    return entry ? &entry->info : NULL;
}

const char* ethervox_language_resolve(const char* tag) {
    if (!tag) return NULL;
    char primary[8];
    size_t n = 0;
    // Primary language subtag: letters up to the first '-', '_' or '.'
    while (tag[n] && tag[n] != '-' && tag[n] != '_' && tag[n] != '.' && n < sizeof(primary) - 1) {
        primary[n] = tag[n];
        n++;
    }
    primary[n] = '\0';
    if (n < 2 || (tag[n] && tag[n] != '-' && tag[n] != '_' && tag[n] != '.')) return NULL;
    const language_entry_t* entry = find_entry(primary);
    return entry ? entry->info.code : NULL;
}

const char* ethervox_language_prompt_block(const char* code) {
    const language_entry_t* entry = find_entry(code);
    return entry ? entry->prompt_block : "";
}
