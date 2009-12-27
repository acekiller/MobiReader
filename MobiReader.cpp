/****************************************************************************
**
** Trolltech hereby grants a license to use the Qt/Eclipse Integration
** plug-in (the software contained herein), in binary form, solely for the
** purpose of creating code to be used with Trolltech's Qt software.
**
** Qt Designer is licensed under the terms of the GNU General Public
** License versions 2.0 and 3.0 ("GPL License"). Trolltech offers users the
** right to use certain no GPL licensed software under the terms of its GPL
** Exception version 1.2 (http://trolltech.com/products/qt/gplexception).
**
** THIS SOFTWARE IS PROVIDED BY TROLLTECH AND ITS CONTRIBUTORS (IF ANY) "AS
** IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
** TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
** PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
** OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
** EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
** PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
** PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
** LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
** NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** Since we now have the GPL exception I think that the "special exception
** is no longer needed. The license text proposed above (other than the
** special exception portion of it) is the BSD license and we have added
** the BSD license as a permissible license under the exception.
**
****************************************************************************/

#include <QFileDialog>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>
#include <QRadioButton>
#include <QString>
#include <QTextStream>

#include "MobiReader.h"

MobiReader::MobiReader(QWidget *parent)
: QMainWindow(parent), first(true) {
    ui.setupUi(this);
    showMaximized();
}

MobiReader::~MobiReader() {

}

void MobiReader::on_myLineEdit_textEdited() {
    ui.myLabel->setText(ui.myLineEdit->text());
}

void MobiReader::setLabelChoise(int choise) {
    ui.myLabel->setText(QString(tr("Chosen %1")).arg(choise));
}
void MobiReader::on_radio1_clicked(bool) {
    setLabelChoise(1);
}

void MobiReader::on_radio2_clicked(bool) {
    setLabelChoise(2);
}

void MobiReader::on_myButton_clicked(bool) {
    if(first) {
        int wH = ui.bookView->size().height();
        int vH = ui.bookView->viewport()->size().height();
        //if(!book.readBook("/home/patryk/FBooks/Jules Verne_A Journey into the Interior of the Earth_(en).mobi")) {
        if(!book.readBook(QFileDialog::getOpenFileName(this, tr("Open")))) {
            QString errorMsg = QString("Error processing eBook!! Error code: %1").arg(book.errorCode());
            QMessageBox::critical(this, tr("Error reading ebook"), errorMsg);
        }
        ui.bookView->setHtml(book.bookText());
        first = false;
    } else {
        int wH = ui.bookView->size().height();
        int vH = ui.bookView->viewport()->size().height();
        qDebug("AAA");
    }
}
