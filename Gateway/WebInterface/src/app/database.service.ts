import { Injectable } from "@angular/core";
import { HttpClient, HttpHeaders } from "@angular/common/http";
const httpOptions = {
  headers: new HttpHeaders({ "Content-Type": "application/json" }),
};

@Injectable({
  providedIn: "root",
})
export class DatabaseService {
  constructor(private http: HttpClient) {}
  result: any;

  createUser(data: any){
    return this.http.post("/registeredusers",data,httpOptions);
  }

  getUsers()
  {
    return this.http.get('/detectedusers');
  }
}