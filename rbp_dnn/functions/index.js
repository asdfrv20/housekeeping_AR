const functions = require('firebase-functions');

const SENDGRID_API_KEY = 'SG.XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX';

const sgMail = require('@sendgrid/mail');
sgMail.setApiKey(SENDGRID_API_KEY);

target = 'Jonathan'
dbRef = '/surveillance/' + target
exports.cloudMailFunction = functions.database.ref(dbRef)
    .onUpdate(( change,context) => {
        console.log(change.after.val())
        const user = change.after.val(); 
        const name = user.name; 
        const time = new Date(user.time*1000);
        const path = user.path;
        
        const message = 'Raspberry Pi Surveillance system detect ' + name 
        var text = `<div>
            <h4>Surveillance system detect ${name || ""}</h4>
            <ul>
                <li>
                Name - ${name || ""}
                </li>
                <li>
                Time - ${time || ""}
                </li>
                <li>
                Dropbox path - ${path || ""}
                </li>
            </ul>
            <h4>Message</h4>
            <p>${message || ""}</p>
        </div>`;
        const msg = {
            to: "creapple@gmail.com",
            from: "no-reply@gmail.com",
            subject: `${name} was detected by Surveillance system`,
            text: text,
            html: text
        };

        return sgMail.send(msg)

    });