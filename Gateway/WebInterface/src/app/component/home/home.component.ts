import { Component, OnInit } from '@angular/core';
import {NgbModal, ModalDismissReasons} from '@ng-bootstrap/ng-bootstrap';
import { DatabaseService } from "../../database.service";
import { Router } from "@angular/router";

@Component({
  selector: 'app-home',
  templateUrl: './home.component.html',
  styleUrls: ['./home.component.css']
})
export class HomeComponent implements OnInit {
  registeredUser: any[] = []; //not needed

  title = 'appBootstrap';
  closeResult!: string;
  
  fName: String = "";
  lName: String = "";
  phoneNum: String = "";
  email: String = "";
  RFID: String = "";


  constructor(private modalService: NgbModal,private dbService: DatabaseService, private router: Router) {}
    
  open(content: any) {
    this.modalService.open(content, {ariaLabelledBy: 'modal-basic-title'}).result.then((result) => {
      this.closeResult = `Closed with: ${result}`;
    }, (reason) => {
      this.closeResult = `Dismissed ${this.getDismissReason(reason)}`;
    });
  }
  
  private getDismissReason(reason: any): string {
    if (reason === ModalDismissReasons.ESC) {
      return 'by pressing ESC';
    } else if (reason === ModalDismissReasons.BACKDROP_CLICK) {
      return 'by clicking on a backdrop';
    } else {
      return  `with: ${reason}`;
    }
  }

 //function to save user when button is clicked
  onSaveUser() {
    let obj = { fName: this.fName, lName: this.lName, phoneNum:this.phoneNum, email:this.email, RFID: this.RFID };
    this.dbService.createUser(obj).subscribe(result => {
      this.modalService.dismissAll();
    });
  }


  ngOnInit(): void {
  }

}
