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
      const region = formData.get("region").trim();

      if (!id && !((name && surname) || (surname && region))) {
        showStatus(
          "Please provide either ID, or Name + Surname, or Surname + Region.",
          "error"
        );
        return;
      }

      const data = {};
      if (name) data.name = name;
      if (surname) data.surname = surname;
      if (id) data.id = id;
      if (region) data.region = region;

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

        showStatus(`Search completed. ${results.length} result(s) found.`, "success");

        results.forEach((user, index) => {
          const card = document.createElement("div");
          card.classList.add("result-card");

          const fields = [
            ["Name", user.name],
            ["Surname", user.surname],
            ["ID", user.id],
            ["Region", user.region],
          ];

          fields.forEach(([label, value]) => {
            const p = document.createElement("p");
            p.innerHTML = `<strong>${label}:</strong> ${value || "—"}`;
            card.appendChild(p);
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