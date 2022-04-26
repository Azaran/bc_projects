using System;
using System.Linq;

namespace IW5Project.Model.Ratings
{
    public class Rating : BaseModel
    {
        public Rating(int stars, string comment)
        {
            if (!Enumerable.Range(1, 10).Contains(stars))
            {
                throw new ArgumentOutOfRangeException("Number of stars must be in range (1,10) and is: " +
                                                      stars.ToString()); // nejaky zakladni osetreni hvezdicek
            }
            Comment = comment;
            Stars = stars;
        }

        public int Stars { get; private set; }
        public string Comment { get; private set; }
      //  public Guid IdRating { get; set; }
        /// <summary>
        /// Change Rating properties
        /// </summary>
        /// <param name="stars">New star count</param>
        /// <param name="comment">New comment</param>
       
        public void EditRating(int stars, string comment = null)
        {
            if (!string.IsNullOrWhiteSpace(comment))
                Comment = comment;
            Stars = stars;

        }
    }
}
