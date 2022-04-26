using System;
using System.Collections.Generic;
using System.Linq;
using IW5Project.Services;
using IW5Project.Model;
using IW5Project.Model.Movies;
using IW5Project.Model.Persons;
using IW5Project.Model.Ratings;


namespace IW5Project
{
    internal static class Program
    {

        [STAThread]
        private static void Main(string[] args)
        {

            //vytvoreni uloziste
            var storage = new Storage();
            var fileManager = new FileManager();
            // vlozeni noveho filmu do uloziste
            storage.Movies.Add(new Movie
            {
                OrigName = "Point Break",
                Description = "Super film",
                Country = "Vsudemozne",
                Genre = "Akcni",
                Name = "Bod zlomu",
                Length = 85,
                AvrRating = 0,
                Actors = null,
                Directors = null,
                Ratings = new List<Rating>()
                
            });

            // pridani hodnoceni k prvnimu prvku v seznamu
            storage.Movies.First().AddRating(new Rating(7, "Co já vím co sem budem psát?"));
            storage.Movies.First().AddRating(new Rating(4, "Třeba tohle"));
            storage.Movies.First().AddRating(new Rating(6, "Hodnocení nějak funguje"));
            var rating = new Rating(8, "něco dalšího");
            storage.Movies.First().AddRating(rating);

            // tisk informací k poslednímu prvku seznamu
            var idMovie = storage.Movies.Last().Id;
            Console.WriteLine($"Originalni nazev: {storage.Movies.Last().OrigName}");
            Console.WriteLine($"Popis: {storage.Movies.Last().Description}");
            Console.WriteLine($"Zeme: {storage.Movies.Last().Country}");
            Console.WriteLine($"Nazev: {storage.Movies.Last().Name}");
            Console.WriteLine($"Delka: {storage.Movies.Last().Length} minut");
            Console.WriteLine($"Prumerne hodnoceni: {storage.Movies.Last().AvrRating} hvezdicek");
            Console.ReadKey();
            Console.WriteLine();


            // nahrani posteru k filmu
            Console.WriteLine();
            Console.WriteLine($"Po zmacknuti klavesy budete vyzvani k nahrani posteru");
            Console.ReadKey();
            FileManager.StatusOfLoadingFile result = fileManager.GetFile(FileManager.TypeOfFileLoading.Poster, storage.Movies.Last().Id);
            if (result == FileManager.StatusOfLoadingFile.Ok)
            {
                storage.Movies.Last().SetPoster(fileManager.Extension);
            }
            else if (result == FileManager.StatusOfLoadingFile.WrongFormat)
                Console.WriteLine($"je podporovan pouze jpg, png,");
            else if (result == FileManager.StatusOfLoadingFile.NoFileSelected)
                Console.WriteLine($"Nastaly I/O problemy");
            Console.WriteLine($"Po zmacknuti klavesy smazu poster");
            Console.ReadKey();
            fileManager.DeleteFile(FileManager.TypeOfFileLoading.Poster, fileManager.Extension, storage.Movies.Last().Id);


            // tisk hodnocení pro první film v seznamu:
            foreach (var r in storage.Movies.First().Ratings)
            {
                Console.WriteLine($"ID: {r.Id}");
                Console.Write($"Hodnocení: ");
                Console.ForegroundColor = ConsoleColor.Green;
                for (var i = 0; i < r.Stars; i++)
                {
                    Console.Write("*");
                }
                Console.ResetColor();
                Console.Write("\n");
                Console.WriteLine(r.Comment);


            }
            //smaze vybrane hodnoceni, metode RemoveRating je predavan objekt s hodnocením, který chceme smazat
            storage.Movies.First().RemoveRating(rating);

            Console.ReadKey();
            Console.WriteLine();
            Console.WriteLine("Mazu jedno hodnoceni...");
            Console.WriteLine();
            Console.WriteLine($"Nove prumerne hodnoceni: {storage.Movies.Last().AvrRating} hvezdicek");

            // tisk hodnocení pro první film v seznamu:
            foreach (var r in storage.Movies.First().Ratings)
            {
                Console.WriteLine($"ID: {r.Id}");
                Console.Write($"Hodnocení: ");
                Console.ForegroundColor = ConsoleColor.Green;
                for (var i = 0; i < r.Stars; i++)
                {
                    Console.Write("*");
                }
                Console.ResetColor();
                Console.Write("\n");
                Console.WriteLine(r.Comment);


            }

            Console.ReadKey();
            //pridani nový osoby
            storage.Persons.Add(new Person("Vojtech", "Vecera", 22));
            var idPerson = storage.Persons.Last().Id;
            //tisk posledni vytvorene osoby
            Console.WriteLine($"Jmeno: {storage.Persons.Last().Lastname}");
            Console.WriteLine($"Prijmeni: {storage.Persons.Last().Lastname}");
            Console.WriteLine($"Vek: {storage.Persons.Last().Age}");

            // nahrani avatara k osobe
            Console.WriteLine();
            Console.WriteLine($"Po zmacknuti klavesy budete vyzvani k nahrani Avatara k osobe");
            Console.ReadKey();
            result = fileManager.GetFile(FileManager.TypeOfFileLoading.Avatar, storage.Movies.Last().Id);
            if (result == FileManager.StatusOfLoadingFile.Ok)
            {
                Console.WriteLine($"Nahrani probehlo uspesne");
                storage.Movies.Last().SetPoster(fileManager.Extension);
            }
            else if (result == FileManager.StatusOfLoadingFile.WrongFormat)
                Console.WriteLine($"je podporovan pouze jpg, png,");
            else if (result == FileManager.StatusOfLoadingFile.NoFileSelected)
                Console.WriteLine($"Nastaly problemy v I/O");


            //vytvoreni vazby rezisera mezi osobou a filmem (Vojtech Vecera reziroval Point break)
            storage.AddAdditionalInfo(Storage.Position.Director, storage.Movies.Last().Id, storage.Persons.Last().Id);
            //tisk id osoby (Vojtech Vecera), id filmu (Point break)
            Console.WriteLine($"ID osoby: {idPerson}");
            Console.WriteLine($"ID filmu: {idMovie}");
            Console.WriteLine();
            Console.ReadKey();
            //vyhledani jestli Vojtech Vecera reziroval film Point break
            if (!storage.AdditionalInfo.SingleOrDefault(p => p.Position == Storage.Position.Director)
                    .MPRelationships.SingleOrDefault(r => r.IdPerson == idPerson && r.IdMovie == idMovie)
                    .Equals(null))
            {
                Console.WriteLine($"{idPerson} pracoval na filmu {idMovie} jako {Storage.Position.Director}");
            }
            Console.WriteLine();
            Console.ReadKey();
            //vytvoreni vazby herce mezi osobou a filmem (Vojtech Vecera hral v Point breaku)
            storage.AddAdditionalInfo(Storage.Position.Actor, storage.Movies.Last().Id, storage.Persons.Last().Id);

            //vyhledani jestli Vojtech Vecera hral ve filmu Point break
            if (!storage.AdditionalInfo.SingleOrDefault(p => p.Position == Storage.Position.Actor)
                    .MPRelationships.SingleOrDefault(r => r.IdPerson == idPerson && r.IdMovie == idMovie)
                    .Equals(null))
            {
                Console.WriteLine($"{idPerson} pracoval na filmu {idMovie} jako {Storage.Position.Actor}");
            }
            Console.ReadKey();

        }
    }
}
