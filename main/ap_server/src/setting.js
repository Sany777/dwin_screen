const LIST_DAY = ["Monday","Thusday","Wednesday","Thursday","Friday","Saturday","Sanday"]

const FORMS_LIST = [
  [["Notification", LIST_DAY],["time",]],
  [["Network",["SSID", "Password"]],["text","32"]],
  [["API", ["City", "Key"]],["text","32"]],
  [["Update", ["Firmware"]],["file",]],
]

function getSetting()
{
  fetch("/data?", {
    method:"GET",
    mode: "no-cors",
    body: null,
  })
  .then((r) => r.json())
  .then((r) => {
    for(const key in r){
      const value = r[key];
      if(key === "Notification") {
        const arrVal = value.split(",");
        LIST_DAY.forEach((nameIn,i)=>{
          const inputNot = document.getElementById(nameIn);
          inputNot.value = arrVal[i];
        })
       } else {
         const input = document.getElementById(key);
         if(input)input.value = value;
      }
    }
   })
  .catch((e) => showModal(e));
}

function createForms()
{
  const containerForms = document.getElementById("settings_forms");
  FORMS_LIST.forEach((data, i)=>{
    const [name, listInput] = data[0];
    const [type, length] = data[1];
    const form = document.createElement("form");
    const container = document.createElement("div");
    const wrapText = document.createElement("h3");
    wrapText.innerText = name;
    form.name = name;
    container.appendChild(wrapText);
    form.action = "";
    const submit = document.createElement("input");
    submit.value = "Submit";
    submit.type = "button";
    submit.onclick = getDataForm;
    listInput.forEach(inputName=>{
      const input = document.createElement("input");
      const label = document.createElement("label");
      label.innerText = inputName;
      input.type = type;
      input.name = inputName;
      input.id = inputName;
      input.value = "";
      if(type === "text"){
        input.maxLength = length;
        input.placeholder = "Enter "+ inputName;
      }
      label.appendChild(input);
      form.appendChild(label);
    })
    form.appendChild(submit);
    container.appendChild(form);
    containerForms.appendChild(container);
  });
}

function updateTime() 
{
  const data = "" + Math.trunc(new Date().getTime() / 1000);
  sendDataForm("time", data);
}

async function getInfo()
{
  await fetch("/getinfo", {
    method: "GET",
    mode: "no-cors",
    body: null,
  })
    .then((r) => r.json())
    .then((r) => showModal("version idf: " + r.version + "\n"
               +"chip: " + r.chip + "\n"
               + "revision: " + r.revision, true))
    .catch((e) => showModal(e, false));
}

createForms();