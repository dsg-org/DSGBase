const form = document.getElementById("searchForm");
const button = form.querySelector("button");
const container = document.querySelector(".userinformation");
const statusMessage = document.getElementById("statusMessage");

let isRequestInProgress = false;

function showStatus(text, type = "info") {
  statusMessage.textContent = text;
  statusMessage.className = "status";
  statusMessage.style.display = "block";
  if (type === "error") statusMessage.classList.add("error");
  if (type === "success") statusMessage.classList.add("success");
}

form.addEventListener("submit", async function (e) {
  e.preventDefault();
  if (isRequestInProgress) return;

  const formData = new FormData(form);
  const name = formData.get("name").trim();
  const surname = formData.get("surname").trim();
  const id = formData.get("id").trim();
  const district = formData.get("district").trim();

  if (!id && !((name && surname) || (surname && district))) {
    showStatus(
      "Please provide either ID, or Name + Surname, or Surname + Region.",
      "error",
    );
    return;
  }

  const data = {};
  if (name) data.name = name;
  if (surname) data.surname = surname;
  if (id) data.id = id;
  if (district) data.district = district;

  isRequestInProgress = true;
  button.disabled = true;
  container.innerHTML = "";
  showStatus("Search in progress. Please wait...", "info");

  try {
    const response = await fetch("/api/search", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
        Authorization: "Bearer 12345",
      },
      body: JSON.stringify(data),
    });

    const results = await response.json();

    if (response.status === 429) {
      showStatus(results.error, "error");
      return;
    }

    if (!Array.isArray(results) || results.length === 0) {
      showStatus("No matching records found.", "info");
      return;
    }

    showStatus(
      `Search completed. ${results.length} result(s) found.`,
      "success",
    );

    results.forEach((user, index) => {
      const card = document.createElement("div");
      card.classList.add("result-card");

      const fields = [
        ["Name", user.name],
        ["Surname", user.surname],
        ["ID", user.id],
        [
          "Region",
          user.district && user.district !== "ნან" ? user.district : null,
        ],
        ["Father", user.father ? user.father : null],
        [
          "Father_ID",
          user.father_id && user.father_id !== "0" && user.father_id !== 0
            ? user.father_id
            : null,
        ],
        ["Mother", user.mother ? user.mother : null],
        [
          "Mother_ID",
          user.mother_id && user.mother_id !== "0" && user.mother_id !== 0
            ? user.mother_id
            : null,
        ],
      ];

      fields.forEach(([label, value]) => {
        if (value && value.toString().trim() !== "" && value !== "0") {
          const row = document.createElement("div");
          row.className = "field-row";

          row.innerHTML = `<strong>${label}:</strong> <span>${value}</span>`;
          card.appendChild(row);
        }
      });

      container.appendChild(card);
    });
  } catch (err) {
    console.error(err);
    showStatus("An error occurred while processing your request.", "error");
  } finally {
    isRequestInProgress = false;
    button.disabled = false;
  }
});
