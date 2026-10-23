// Notionのデータベースに「テスト」という行を1件追加するだけのスクリプト。
// 使い方: プロジェクトの一番上のフォルダで  node tools/notion/add-test-row.mjs
// 必要: .env に NOTION_TOKEN と NOTION_DB_ID を書いておく（Node 20.12以上）
import path from "node:path";
import { fileURLToPath } from "node:url";

const here = path.dirname(fileURLToPath(import.meta.url));
const envPath = path.resolve(here, "../../.env");
try {
  process.loadEnvFile(envPath);
} catch {
  console.error(`.env が読めません: ${envPath}`);
  process.exit(1);
}

const token = process.env.NOTION_TOKEN?.trim();
const dbId = process.env.NOTION_DB_ID?.trim();
if (!token || !dbId) {
  console.error("NOTION_TOKEN と NOTION_DB_ID の両方を .env に書いてください。");
  process.exit(1);
}

const headers = {
  Authorization: `Bearer ${token}`,
  // 2025-09-03 以降はデータベースの中の「データソース」に行を追加する仕組み
  "Notion-Version": "2025-09-03",
  "Content-Type": "application/json",
};

async function call(url, options) {
  const res = await fetch(url, { ...options, headers });
  const body = await res.json();
  if (!res.ok) {
    console.error(`失敗 (${res.status}): ${body.message ?? JSON.stringify(body)}`);
    if (res.status === 404) {
      console.error("→ データベースに、作成したコネクトを「接続」しましたか？IDは合っていますか？");
    }
    if (res.status === 401) {
      console.error("→ トークンが違うようです。");
    }
    process.exit(1);
  }
  return body;
}

// 1) データベースから最初のデータソースを取り出す
const db = await call(`https://api.notion.com/v1/databases/${dbId}`, { method: "GET" });
const dataSourceId = db.data_sources?.[0]?.id;
if (!dataSourceId) {
  console.error("データソースが見つかりません。");
  process.exit(1);
}

// 2) データソースの列を調べて、タイトル列の名前を見つける
const ds = await call(`https://api.notion.com/v1/data_sources/${dataSourceId}`, { method: "GET" });
const titleName = Object.entries(ds.properties).find(([, p]) => p.type === "title")?.[0];
if (!titleName) {
  console.error("タイトル列が見つかりません。");
  process.exit(1);
}
console.log(`データベース: ${db.title?.[0]?.plain_text ?? "(無題)"} / データソース: ${db.data_sources[0].name ?? "(無題)"} (全${db.data_sources.length}件) / タイトル列: ${titleName}`);

// 3) テスト行を1件追加
const page = await call("https://api.notion.com/v1/pages", {
  method: "POST",
  body: JSON.stringify({
    parent: { type: "data_source_id", data_source_id: dataSourceId },
    properties: {
      [titleName]: { title: [{ text: { content: "【テスト】自動追加（消してOK）" } }] },
    },
  }),
});
console.log("追加しました:", page.url);
