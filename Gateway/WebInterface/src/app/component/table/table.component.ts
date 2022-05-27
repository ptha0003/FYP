import { Component, OnInit } from '@angular/core';
import { DatabaseService } from "../../database.service";

@Component({
  selector: 'app-table',
  templateUrl: './table.component.html',
  styleUrls: ['./table.component.css']
})
export class TableComponent implements OnInit {
  public usersDB: any[] = []; //store database data into this array
  constructor(private dbService: DatabaseService) { }

  ngOnInit(): void {
    this.dbService.getUsers().subscribe((data: any) => {
      this.usersDB = data;
    });
  }

}
